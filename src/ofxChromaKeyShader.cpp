/********************************************************************************** 
 
 Copyright (c) 2013- Koo Kin Yat, Eric (musiko)
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 **********************************************************************************/

#include "ofxChromaKeyShader.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
ofxChromaKeyShader::ofxChromaKeyShader(int _width, int _height)
{
	width = _width;
	height = _height;

	fbo_detail.allocate(width, height, GL_RGB);
	fbo_base.allocate(width, height, GL_RGB);
	fbo_chroma.allocate(width, height, GL_RGB);
	fbo_pingpong.allocate(width, height, GL_RGBA);
	fbo_final.allocate(width, height, GL_RGBA);

	paramGp.setName("greenscreenSettings");
	paramGp.add(bgColorPos.set("bgColorPos", ofVec2f(width*0.75, height*0.1), ofVec2f(0.0), ofVec2f(width, height)));
	paramGp.add(bgColorSize.set("bgColorROIsize", 20, 1, height));
	bgColor.set("bgColor", ofFloatColor(0.5020, 0.7373, 0.5137, 1.0), ofFloatColor(0.0, 0.0), ofFloatColor(1.0, 1.0));
	paramGp.add(bgColor);
	paramGp.add(baseMaskStrength.set("baseMaskStrength", .6, 0.0, 1.0));
	paramGp.add(chromaMaskStrength.set("chromaMaskStrength", .4, 0.0, 1.0));	
	paramGp.add(greenSpillStrength.set("greenspillStrength", .4, 0.0, 1.0));
	paramGp.add(multiplyFilterHueOffset.set("multiplyFilterHueOffset", 0.25, 0.0, 1.0));
	paramGp.add(blurValue.set("blurValue", 512.f, 0.0, 4096.f));
	paramGp.add(dilateStep.set("dilateStep", 3, 0, 7));
	paramGp.add(erodeStep.set("erodeStep", 1, 0, 7));
	paramGp.add(baseMaskClip.set("baseMaskClip(b,w)", ofVec2f(.2, .6), ofVec2f(0.0), ofVec2f(1.0)));
	paramGp.add(detailMaskClip.set("detailMaskClip(b,w)", ofVec2f(.565, .82), ofVec2f(0.0), ofVec2f(1.0)));
	paramGp.add(endMaskClip.set("endMaskClip(b,w)", ofVec2f(.1, .6), ofVec2f(0.0), ofVec2f(1.0)));
	paramGp.add(clippingMaskTL.set("clippingMask(TL)", ofVec2f(0.f), ofVec2f(0.f), ofVec2f(width, height)));
	paramGp.add(clippingMaskBR.set("clippingMask(BR)", ofVec2f(0.f), ofVec2f(0.f), ofVec2f(width, height)));
	paramGp.add(photoOffset.set("photoOffset", ofVec2f(0.f), ofVec2f(-width, -height), ofVec2f(width, height)));
	paramGp.add(photoZoom.set("photoZoom", 1.f, 1.f, 4.f));
	
	loadShaders();
}

//--------------------------------------------------------------
ofxChromaKeyShader::~ofxChromaKeyShader(void)
{
}

//--------------------------------------------------------------
void ofxChromaKeyShader::drawDetailMask(float x, float y, float w, float h){
	fbo_detail.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxChromaKeyShader::drawBaseMask(float x, float y, float w, float h){
	fbo_base.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxChromaKeyShader::drawChromaMask(float x, float y, float w, float h){
	fbo_chroma.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxChromaKeyShader::drawFinalMask(float x, float y, float w, float h){
	fbo_pingpong.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxChromaKeyShader::drawFinalImage(float x, float y, float w, float h){
	fbo_final.draw(x, y, w, h);
}

//--------------------------------------------------------------
ofImage ofxChromaKeyShader::getFinalImage(){
	ofImage finalImg; ofPixels pix; 
	fbo_final.readToPixels(pix);
	finalImg.setFromPixels(pix);
	finalImg.setImageType(OF_IMAGE_COLOR);
	
	return finalImg;
}

//--------------------------------------------------------------
void ofxChromaKeyShader::loadShaders(){
	// Reload shaders
	shader_detail.unload();
	shader_base.unload();
	shader_hblur.unload();
	shader_vblur.unload();
	shader_dilate.unload();
	shader_erode.unload();
	shader_chroma.unload();
	shader_final.unload();

	ofLogVerbose("ofxChromaKeyShader.loadShaders()") << "Loading shader.detailmask......";
	shader_detail.setupShaderFromSource(GL_FRAGMENT_SHADER, chromakeyshaders.detailmask);
	shader_detail.linkProgram();

	ofLogVerbose("ofxChromaKeyShader.loadShaders()") << "Loading shader.basemask......";
	shader_base.setupShaderFromSource(GL_FRAGMENT_SHADER, chromakeyshaders.basemask);
	shader_base.linkProgram();

	ofLogVerbose("ofxChromaKeyShader.loadShaders()") << "Loading shader.blurHorizontal......";
	shader_hblur.setupShaderFromSource(GL_FRAGMENT_SHADER, chromakeyshaders.blurHorizontal);
	shader_hblur.linkProgram();

	ofLogVerbose("ofxChromaKeyShader.loadShaders()") << "Loading shader.blurVertical......";
	shader_vblur.setupShaderFromSource(GL_FRAGMENT_SHADER, chromakeyshaders.blurVertical);
	shader_vblur.linkProgram();

	ofLogVerbose("ofxChromaKeyShader.loadShaders()") << "Loading shader.dilation......";
	shader_dilate.setupShaderFromSource(GL_FRAGMENT_SHADER, chromakeyshaders.dilation);
	shader_dilate.linkProgram();

	ofLogVerbose("ofxChromaKeyShader.loadShaders()") << "Loading shader.erosion......";
	shader_erode.setupShaderFromSource(GL_FRAGMENT_SHADER, chromakeyshaders.erosion);
	shader_erode.linkProgram();

	ofLogVerbose("ofxChromaKeyShader.loadShaders()") << "Loading shader.chromamask......";
	shader_chroma.setupShaderFromSource(GL_FRAGMENT_SHADER, chromakeyshaders.chromamask);
	shader_chroma.linkProgram();

	ofLogVerbose("ofxChromaKeyShader.loadShaders()") << "Loading shader.finalmask......";
	shader_final.setupShaderFromSource(GL_FRAGMENT_SHADER, chromakeyshaders.finalmask);
	shader_final.linkProgram();

	ofLogVerbose("ofxChromaKeyShader.loadShaders()") << "All shaders loaded";
}

//--------------------------------------------------------------
void ofxChromaKeyShader::updateBgColorPos(float x, float y){
	bgColorPos = ofVec2f(x, y);
}

//--------------------------------------------------------------
void ofxChromaKeyShader::updateBgColor(ofPixelsRef camPixRef){
	ofImage camImg(camPixRef);
	camImg.crop(bgColorPos.get().x, bgColorPos.get().y, bgColorSize, bgColorSize);
	Mat roiMat; roiMat = toCv(camImg);
	cv::Scalar scalar = mean(roiMat);
	bgColor = ofFloatColor(scalar[0]/255, scalar[1]/255, scalar[2]/255);
}

//--------------------------------------------------------------
void ofxChromaKeyShader::updateChromakeyMask(ofTexture input_tex, ofTexture bg_tex){
	// == 1. Detailed mask ==========================================================
	shader_detail.begin();
		// Input params to shader
		shader_detail.setUniformTexture("baseTex", input_tex, 0);
		shader_detail.setUniform4f("bgColor", bgColor.get().r, bgColor.get().g, bgColor.get().b, bgColor.get().a);
		shader_detail.setUniform2f("detailMaskVal", detailMaskClip.get().x, detailMaskClip.get().y);
		// Draw FBO
		fbo_detail.begin();			
			ofSetColor(255,255,255);
			input_tex.draw(0, 0, width, height);			
		fbo_detail.end();
	shader_detail.end();
	// == 2a. Base mask - from cam image ==========================================================
	shader_base.begin();
		
		// Input params to shader
		shader_base.setUniformTexture("baseTex", input_tex, 0);
		shader_base.setUniform2f("baseMaskVal", baseMaskClip.get().x, baseMaskClip.get().y);
		shader_base.setUniform1f("baseMaskStrength", baseMaskStrength);
		// Draw FBO
		fbo_base.begin();			
			ofSetColor(255,255,255);
			input_tex.draw(0, 0, width, height);			
		fbo_base.end();
	shader_base.end();
	// == 2b. Base mask - horizontal blur ==========================================================
	shader_hblur.begin();
		
		// Input params to shader
		shader_hblur.setUniformTexture("baseTex", fbo_base.getTextureReference(), 0);
		shader_hblur.setUniform1f("blurValue", blurValue);
		// Draw FBO
		fbo_pingpong.begin();			
			ofSetColor(255,255,255);
			fbo_base.draw(0, 0, width, height);			
		fbo_pingpong.end();
	shader_hblur.end();
	// == 2c. Base mask - vertical blur ==========================================================
	shader_vblur.begin();
		
		// Input params to shader
		shader_vblur.setUniformTexture("baseTex", fbo_pingpong.getTextureReference(), 0);
		shader_vblur.setUniform1f("blurValue", blurValue);
		// Draw FBO
		fbo_base.begin();			
			ofSetColor(255,255,255);
			fbo_pingpong.draw(0, 0, width, height);			
		fbo_base.end();
	shader_vblur.end();
	// == 2d. Base mask - dilation ==========================================================
	shader_dilate.begin();
		
		// Input params to shader
		shader_dilate.setUniformTexture("baseTex", fbo_base.getTextureReference(), 0);
		shader_dilate.setUniform1i("dilateStep", dilateStep);
		// Draw FBO
		fbo_pingpong.begin();			
			ofSetColor(255,255,255);
			ofClear(0);
			fbo_base.draw(0, 0, width, height);			
		fbo_pingpong.end();
	shader_dilate.end();
	// == 2e. Base mask - erosion ==========================================================
	shader_erode.begin();
		
		// Input params to shader
		shader_erode.setUniformTexture("baseTex", fbo_pingpong.getTextureReference(), 0);
		shader_erode.setUniform1i("erodeStep", erodeStep);
		// Draw FBO
		fbo_base.begin();			
			ofSetColor(255,255,255);
			ofClear(0);
			fbo_pingpong.draw(0, 0, width, height);			
		fbo_base.end();
	shader_erode.end();
	// == 3a. Chroma mask - inverted mask ==========================================================
	shader_chroma.begin();
		
		// Input params to shader
		shader_chroma.setUniformTexture("baseTex", input_tex, 0);
		shader_chroma.setUniform4f("bgColor", bgColor.get().r, bgColor.get().g, bgColor.get().b, bgColor.get().a);
		shader_chroma.setUniform1f("multiplyFilterHueOffset", multiplyFilterHueOffset);
		// Draw FBO
		fbo_chroma.begin();			
			ofSetColor(255,255,255);
			ofClear(0);
			input_tex.draw(0, 0, width, height);			
		fbo_chroma.end();
	shader_chroma.end();
	// == 3b. Chroma mask - horizontal blur ==========================================================
	shader_hblur.begin();
		
		// Input params to shader
		shader_hblur.setUniformTexture("baseTex", fbo_chroma.getTextureReference(), 0);
		shader_hblur.setUniform1f("blurValue", blurValue);
		// Draw FBO
		fbo_pingpong.begin();			
			ofSetColor(255,255,255);
			fbo_chroma.draw(0, 0, width, height);			
		fbo_pingpong.end();
	shader_hblur.end();
	// == 3c. Chroma mask - vertical blur ==========================================================
	shader_vblur.begin();
		
		// Input params to shader
		shader_vblur.setUniformTexture("baseTex", fbo_pingpong.getTextureReference(), 0);
		shader_vblur.setUniform1f("blurValue", blurValue);
		// Draw FBO
		fbo_chroma.begin();			
			ofSetColor(255,255,255);
			fbo_pingpong.draw(0, 0, width, height);			
		fbo_chroma.end();
	shader_vblur.end();
	// == 4. Final mask - Green spill, chroma mask and combine all =================================
	shader_final.begin();		
		// Input params to shader
		shader_final.setUniformTexture("baseTex", input_tex, 0);
		shader_final.setUniformTexture("baseMaskTex", fbo_base.getTextureReference(), 1);
		shader_final.setUniformTexture("detailMaskTex", fbo_detail.getTextureReference(), 2);
		shader_final.setUniformTexture("inverted_chromaMaskTex", fbo_chroma.getTextureReference(), 3);
		shader_final.setUniform4f("bgColor", bgColor.get().r, bgColor.get().g, bgColor.get().b, bgColor.get().a);
		shader_final.setUniform1f("multiplyFilterHueOffset", multiplyFilterHueOffset);
		shader_final.setUniform2f("endMaskVal", endMaskClip.get().x, endMaskClip.get().y);
		shader_final.setUniform1f("greenSpillStrength", greenSpillStrength);
		shader_final.setUniform1f("chromaMaskStrength", chromaMaskStrength);
		// Draw FBO
		fbo_pingpong.begin();
			ofClear(0.f, 0.f);			
			ofSetColor(255,255,255);
			input_tex.draw(0, 0, width, height);
		fbo_pingpong.end();
	shader_final.end();

	// == 5. Combine with BG =================================
	float clipX = clippingMaskTL.get().x;
	float clipY = clippingMaskTL.get().y;
	float clipW = clippingMaskBR.get().x - clippingMaskTL.get().x;
	float clipH = clippingMaskBR.get().y - clippingMaskTL.get().y;
	fbo_final.begin();
		ofClear(0.f, 0.f);
		ofSetColor(255,255,255);
		if(bg_tex.isAllocated())
			bg_tex.draw(0, 0, width, height);
		ofPushMatrix();
			ofTranslate(width/2, height/2);
			ofScale(photoZoom, photoZoom);
			fbo_pingpong.getTextureReference().drawSubsection(photoOffset.get().x + clipX - width/2, photoOffset.get().y + clipY - height/2, clipW, clipH, clipX, clipY, clipW, clipH);
		ofPopMatrix();
	fbo_final.end();
}