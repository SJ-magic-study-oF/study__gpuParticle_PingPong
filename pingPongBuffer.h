/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
// examples/gl/gpuParticleSystemExampleを元に、複数バッファ書き出しに対応

struct pingPongBuffer {
public:
	void allocate( int _width, int _height, int _internalformat = GL_RGBA, int _numColorBuffers = 1){
		// Allocate
		ofFbo::Settings fboSettings;
		fboSettings.width  = _width;
		fboSettings.height = _height;
		fboSettings.numColorbuffers = _numColorBuffers;
		fboSettings.useDepth = false;
		fboSettings.internalformat = _internalformat;	// Gotta store the data as floats, they won't be clamped to 0..1
		fboSettings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
		fboSettings.wrapModeVertical = GL_CLAMP_TO_EDGE;
		fboSettings.minFilter = GL_NEAREST; // No interpolation, that would mess up data reads later!
		fboSettings.maxFilter = GL_NEAREST;
		for(int i = 0; i < 2; i++){
			FBOs[i].allocate(fboSettings );
		}
		
		// Clean
		clear();
	}
	
	void swap(){
		std::swap(src,dst);
	}
	
	void clear(){
		for(int i = 0; i < 2; i++){
			FBOs[i].begin();
			ofClear(0,255);
			FBOs[i].end();
		}
	}
	
	ofFbo& operator[]( int n ){ return FBOs[n];}
	ofFbo   *src = &FBOs[0];	   // Source	   ->  Ping
	ofFbo   *dst = &FBOs[1];	   // Destination  ->  Pong
	
private:
	ofFbo   FBOs[2];
};


