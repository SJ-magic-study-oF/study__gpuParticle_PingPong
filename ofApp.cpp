/************************************************************
************************************************************/
#include "ofApp.h"


/************************************************************
************************************************************/

//--------------------------------------------------------------
void ofApp::setup(){
	/********************
	********************/
	ofSetWindowTitle("particle test");
	ofSetWindowShape(1280, 720);
	
	ofSetVerticalSync(false);
	ofSetFrameRate(60);
	
	ofBackground(0, 0, 0);
	ofDisableAlphaBlending();
	
	/********************
	********************/
	cam.setupPerspective();
	
	particleNum = 1000000;
	texRes = ceil(sqrt(particleNum));
	
	render.load("shaders/render");
	updatePos.load("","shaders/update.frag");
	
	particles.setMode(OF_PRIMITIVE_POINTS);
	for(int i=0;i<texRes;i++){
		for(int j=0;j<texRes;j++){
			int index = i * texRes + j;
			if(index < particleNum){
				particles.addVertex(ofVec3f(0,0,0));
				particles.addTexCoord(ofVec2f(i, j)); // Fboのテクスチャー内で、データを読み出す位置を設定
				particles.addColor(ofFloatColor(0.1,0.5,0.5,0.5));
			}
		}
	}
	
	/********************
	********************/
	// パーティクルの座標・加速度の保存用Fbo
	// RGBA32Fの形式で2つのColorbufferを用意
	pingPong.allocate(texRes, texRes, GL_RGBA32F, 2);
	
	// パーティクルの位置と経過時間の初期設定
	float * posAndAge = new float[texRes * texRes * 4];
	for (int x = 0; x < texRes; x++){
		for (int y = 0; y < texRes; y++){
			int i = texRes * y + x;
			posAndAge[i*4 + 0] = ofRandom(-1.0,1.0);
			posAndAge[i*4 + 1] = ofRandom(-1.0,1.0);
			posAndAge[i*4 + 2] = ofRandom(-1.0,1.0);
			posAndAge[i*4 + 3] = 0;
		}
	}
	//pingPongBufferに初期値を書き込み
	pingPong.src->getTextureReference(0).loadData(posAndAge, texRes, texRes, GL_RGBA);
	delete [] posAndAge;
	
	/********************
	********************/
	// パーティクルの速度と生存期間の初期設定
	float * velAndMaxAge = new float[texRes * texRes * 4];
	for (int x = 0; x < texRes; x++){
		for (int y = 0; y < texRes; y++){
			int i = texRes * y + x;
			velAndMaxAge[i*4 + 0] = 0.0;
			velAndMaxAge[i*4 + 1] = 0.0;
			velAndMaxAge[i*4 + 2] = 0.0;
			velAndMaxAge[i*4 + 3] = ofRandom(1,150);
		}
	}

	//pingPongBufferに初期値を書き込み
	pingPong.src->getTextureReference(1).loadData(velAndMaxAge, texRes, texRes, GL_RGBA);
	delete [] velAndMaxAge;

	/********************
	********************/
	showTex = false;
}

//--------------------------------------------------------------
void ofApp::update(){
	float time = ofGetElapsedTimef();
	
	// パーティクルの発生位置を更新
	prevEmitterPos = emitterPos;
	emitterPos = 300 * ofVec3f(ofSignedNoise(time, 0, 0),ofSignedNoise(0, time, 0),ofSignedNoise(0, 0, time));
	
	// パーティクルの位置を計算
	pingPong.dst->begin();
		// 複数バッファの書き出しを有効化
		pingPong.dst->activateAllDrawBuffers();
		ofClear(0);
		
		updatePos.begin();
	
		updatePos.setUniformTexture("u_posAndAgeTex", pingPong.src->getTextureReference(0), 0);		// パーティクルの位置と経過時間
		updatePos.setUniformTexture("u_velAndMaxAgeTex", pingPong.src->getTextureReference(1), 1);	// パーティクルの速度と生存期間
		
		/********************
		u_timestep
			大:	時間の経過に対してperlin noise座標が大きく離れる.
				具体的には、スーッと流れるラインが細切れになっていく.
			
		u_scale
			小:	posが移動していっても、perlin noise座標が離れていかないので、perlin noiseに沿って動く.
			大:	posの移動に伴って、perlin noise座標が大きく離れていくので、動作の雰囲気がrandomに近く.
		********************/
		updatePos.setUniform1f("u_time", time);
		updatePos.setUniform1f("u_timestep", 0.5);
		updatePos.setUniform1f("u_scale", 0.005);
		updatePos.setUniform3f("u_emitterPos", emitterPos.x, emitterPos.y, emitterPos.z);
		updatePos.setUniform3f("u_prevEmitterPos", prevEmitterPos.x, prevEmitterPos.y, prevEmitterPos.z);
		
		pingPong.src->draw(0, 0);
		
		updatePos.end();
	pingPong.dst->end();
	
	pingPong.swap();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();
	
	cam.begin();
	render.begin();
	
		render.setUniformTexture("u_posAndAgeTex", pingPong.src->getTextureReference(0), 0); // パーティクルの位置と経過時間
		
		// glPointSize(2.0); // ここで指定してもダメ. ∵render.vert で指定しているから.
		particles.draw();
	render.end();
	cam.end();
	
	ofDisablePointSprites();
	ofPopStyle();
	
	if(showTex){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		pingPong.dst->getTextureReference(0).draw(0,0);
		ofDrawBitmapStringHighlight("Position", 0,14);
		pingPong.dst->getTextureReference(1).draw(texRes,0);
		ofDrawBitmapStringHighlight("Velocity", texRes,14);
		ofPopStyle();
	}
	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 0,ofGetHeight() - 2);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case 's':
			showTex = !showTex;
			break;
			
		case 'f':
			ofToggleFullscreen();
			break;
			
		case ' ':
			ofSaveScreen("image.png");
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
