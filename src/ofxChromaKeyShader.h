#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ChromeKeyShaders.h"

class ofxChromaKeyShader
{
public:
	// ==================== Methods ====================

	ofxChromaKeyShader(int _width, int _height);
	~ofxChromaKeyShader(void);

	void loadShaders();
	void updateBgColorPos(float x, float y);
	void updateBgColor(ofPixelsRef camPixRef);
	void updateChromakeyMask(ofTexture input_tex, ofTexture bg_tex);

	void drawDetailMask(float x, float y, float w, float h);
	void drawBaseMask(float x, float y, float w, float h);
	void drawChromaMask(float x, float y, float w, float h);
	void drawFinalMask(float x, float y, float w, float h);
	void drawFinalImage(float x, float y, float w, float h);

	ofImage getFinalImage();

	// ==================== Variables ====================
	ChromaKeyShaders chromakeyshaders;
	int width, height;

	// Shader & FBO
	ofShader shader_detail, shader_base, shader_hblur, shader_vblur;
	ofShader shader_dilate, shader_erode, shader_chroma, shader_final;
	ofFbo fbo_detail, fbo_base, fbo_pingpong, fbo_chroma, fbo_final;
			
	// Greenscreen params
	ofParameterGroup paramGp;

	ofParameter<ofVec2f> bgColorPos;
	ofParameter<float> bgColorSize;
	ofParameter<ofFloatColor> bgColor;

	ofParameter<ofVec2f> baseMaskClip;
	ofParameter<ofVec2f> detailMaskClip;
	ofParameter<ofVec2f> endMaskClip;

	ofParameter<float> baseMaskStrength;
	ofParameter<float> chromaMaskStrength;
	ofParameter<float> greenSpillStrength;

	ofParameter<float> blurValue;
	ofParameter<float> multiplyFilterHueOffset;

	ofParameter<int> dilateStep;
	ofParameter<int> erodeStep;

	ofParameter<ofVec2f> clippingMaskTL;
	ofParameter<ofVec2f> clippingMaskBR;

	ofParameter<ofVec2f> photoOffset;
	ofParameter<float> photoZoom;
};

