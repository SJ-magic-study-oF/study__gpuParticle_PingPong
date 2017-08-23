/************************************************************
■参考URL
	openFrameworksでGPUパーティクル
		http://yasuhirohoshino.com/archives/30
		
	curl noise
		https://syphobia.wordpress.com/2011/04/12/curl-noise-for-particles/
		http://blog.livedoor.jp/akinow/archives/52378824.html
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "pingPongBuffer.h"

/************************************************************
************************************************************/
class ofApp : public ofBaseApp{
private:
	ofVboMesh particles;
	ofShader render, updatePos;
	ofEasyCam cam;
	pingPongBuffer pingPong;
	ofVec3f emitterPos = ofVec3f(0, 0, 0);
	ofVec3f prevEmitterPos;
	int particleNum, texRes;
	bool showTex;

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
};
