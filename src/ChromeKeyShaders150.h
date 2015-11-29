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

#pragma once

#define STRINGIFY(A) #A

#include "ofMain.h"

class ChromaKeyShaders150 {
public:
	string vertex, basemask, blurHorizontal, blurVertical, chromamask, detailmask, dilation, erosion, finalmask;

	ChromaKeyShaders150() {
        vertex = "#version 150\n";
        vertex += STRINGIFY(
                            uniform mat4 projectionMatrix;
                            uniform mat4 modelViewMatrix;
                            uniform mat4 modelViewProjectionMatrix;
                            
                            in vec4  position;
                            in vec2  texcoord;
                            
                            out vec2 texCoordVarying;
                            
                            void main()
                            {
                                texCoordVarying = texcoord;
                                gl_Position = modelViewProjectionMatrix * position;
                            }
                            );
        
		// === Base mask ============================================================================
		basemask = "#version 150\n";
		basemask += STRINGIFY(
			// Input params
			uniform sampler2DRect baseTex;
			uniform vec2 baseMaskVal;
			uniform float baseMaskStrength;
                              
            in vec2 texCoordVarying;
                              
            out vec4 fragColor;

			// Texture coordinates
			vec2 texcoord0 = texCoordVarying;

			// Temp. results variables
			vec3 baseMask;
			vec3 red;
			vec3 green;
			vec3 blue;
			vec3 redSub;
			vec3 greenSub;
			vec3 blueSub;

			void main(){
				vec4 image = texture(baseTex, texcoord0);
		
				// Split RGB channel
				red = vec3(image.r);
				green = vec3(image.g);
				blue = vec3(image.b);
		
				// Create the base mask: green minus red, invert it
				baseMask = max(green - red, 0.0);
				baseMask = max(1.0 - baseMask, 0.0) - max(1.0 - vec3(baseMaskStrength), 0.0);
				// Clip base mask
				vec3 basemask_min = vec3(baseMaskVal.x);
				vec3 basemask_max = vec3(baseMaskVal.y);
				baseMask = clamp(baseMask, basemask_min, basemask_max);
				baseMask = (baseMask - basemask_min) / (basemask_max - basemask_min);
		
				// Result
				fragColor = vec4(baseMask, 1.0);
			}
		);

		// === Horizontal blur ============================================================================
		blurHorizontal = "#version 150\n";
		blurHorizontal += STRINGIFY(
			uniform sampler2DRect baseTex;
			uniform float blurValue;

            in vec2 texCoordVarying;
                                    
            out vec4 fragColor;

            // Texture coordinates
            vec2 texcoord0 = texCoordVarying;
	 
			void main(void)
			{
				vec4 sum = vec4(0.0);
				float blurSize = blurValue/512.0;
		
				// Blur in X (horizontal)
				sum += texture(baseTex, vec2(texcoord0.x - 4.0*blurSize, texcoord0.y)) * 0.05;
				sum += texture(baseTex, vec2(texcoord0.x - 3.0*blurSize, texcoord0.y)) * 0.09;
				sum += texture(baseTex, vec2(texcoord0.x - 2.0*blurSize, texcoord0.y)) * 0.12;
				sum += texture(baseTex, vec2(texcoord0.x - blurSize, texcoord0.y)) * 0.15;
				sum += texture(baseTex, vec2(texcoord0.x, texcoord0.y)) * 0.16;
				sum += texture(baseTex, vec2(texcoord0.x + blurSize, texcoord0.y)) * 0.15;
				sum += texture(baseTex, vec2(texcoord0.x + 2.0*blurSize, texcoord0.y)) * 0.12;
				sum += texture(baseTex, vec2(texcoord0.x + 3.0*blurSize, texcoord0.y)) * 0.09;
				sum += texture(baseTex, vec2(texcoord0.x + 4.0*blurSize, texcoord0.y)) * 0.05;

				fragColor = vec4(sum.rgb, 1.0);
			}
		);

		// === Vertical blur ============================================================================
		blurVertical = "#version 150\n";
		blurVertical += STRINGIFY(
			uniform sampler2DRect baseTex;
			uniform float blurValue;

            in vec2 texCoordVarying;
                                  
            out vec4 fragColor;

            // Texture coordinates
            vec2 texcoord0 = texCoordVarying;
	 
			void main(void)
			{
				vec4 sum = vec4(0.0);
				float blurSize = blurValue/512.0;
		
				// Blur in Y (Vertical)
				sum += texture(baseTex, vec2(texcoord0.x, texcoord0.y - 4.0*blurSize)) * 0.05;
				sum += texture(baseTex, vec2(texcoord0.x, texcoord0.y - 3.0*blurSize)) * 0.09;
				sum += texture(baseTex, vec2(texcoord0.x, texcoord0.y - 2.0*blurSize)) * 0.12;
				sum += texture(baseTex, vec2(texcoord0.x, texcoord0.y - blurSize)) * 0.15;
				sum += texture(baseTex, vec2(texcoord0.x, texcoord0.y)) * 0.16;
				sum += texture(baseTex, vec2(texcoord0.x, texcoord0.y + blurSize)) * 0.15;
				sum += texture(baseTex, vec2(texcoord0.x, texcoord0.y + 2.0*blurSize)) * 0.12;
				sum += texture(baseTex, vec2(texcoord0.x, texcoord0.y + 3.0*blurSize)) * 0.09;
				sum += texture(baseTex, vec2(texcoord0.x, texcoord0.y + 4.0*blurSize)) * 0.05;
		   
				fragColor = vec4(sum.rgb, 1.0);
			}
		);

		// === Chroma mask ============================================================================
		chromamask = "#version 150\n";
		chromamask += STRINGIFY(
			// Input params
			uniform sampler2DRect baseTex;
			uniform vec4 bgColor;
			uniform float multiplyFilterHueOffset;

            in vec2 texCoordVarying;
                                
            out vec4 fragColor;
            
            // Texture coordinates
            vec2 texcoord0 = texCoordVarying;

			// Temp. results variables
			vec3 chromaMask;
			vec3 inverted_chromaMask;

			vec3 rgb2hsv(vec3 c)
			{
				vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
				vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
				vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);

				float d = q.x - min(q.w, q.y);
				float e = 1.0e-10;
				return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
			}

			vec3 hsv2rgb(vec3 c)
			{
				vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
				vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
				return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
			}

			void main(void)
			{
				float PI = 3.14159265358979323846264;
		
				vec3 image = texture(baseTex, texcoord0).rgb;	
				// Convert RGB to HSV
				vec3 bgColor_hsv = rgb2hsv(bgColor.rgb);
				float bgColor_hue = bgColor_hsv[0];
				vec3 hsv = rgb2hsv(image);
				float hue = hsv[0];
		
				float f =  sin( 2.0 * PI * ( bgColor_hue + ( multiplyFilterHueOffset - hue )));
				chromaMask = vec3(clamp(f, 0.0, 1.0));
				inverted_chromaMask = max(1.0 - chromaMask, 0.0);
		
				fragColor = vec4(inverted_chromaMask, 1.0);
			}
		);

		// === Detailed mask ============================================================================
		detailmask = "#version 150\n";
		detailmask += STRINGIFY(
			// Input params
			uniform sampler2DRect baseTex;
			uniform vec4 bgColor;
			uniform vec2 detailMaskVal;

            in vec2 texCoordVarying;
                                
            out vec4 fragColor;
            
            // Texture coordinates
            vec2 texcoord0 = texCoordVarying;

			// Temp. results variables
			vec3 detailMask;
			vec3 red;
			vec3 green;
			vec3 blue;
			vec3 redSub;
			vec3 greenSub;
			vec3 blueSub;

			void main(){
				vec4 image = texture(baseTex, texcoord0);
		
				// Split RGB channel, subtract the background from each channel and invert the green
				red = vec3(image.r);
				green = vec3(image.g);
				blue = vec3(image.b);
		
				redSub = max(red - vec3(bgColor.r), 0.0);
				greenSub = max(1.0 - green, 0.0) - max(1.0 - vec3(bgColor.g), 0.0);
				blueSub =  max(blue - vec3(bgColor.b), 0.0);
		
				// Create the detail mask
				detailMask = redSub + blueSub + greenSub;
		
				// Clip detail mask
				vec3 detailmask_min = vec3(detailMaskVal.x);
				vec3 detailmask_max = vec3(detailMaskVal.y);
				detailMask = clamp(detailMask, detailmask_min, detailmask_max);
				detailMask = (detailMask - detailmask_min) / (detailmask_max - detailmask_min);
			
				// Result
				fragColor = vec4(detailMask, 1.0);
			}
		);

		// === Dilation ============================================================================
		dilation = "#version 150\n";
		dilation += STRINGIFY(
			// Input params
			uniform sampler2DRect baseTex;
			uniform int dilateStep;

            in vec2 texCoordVarying;
                              
            out vec4 fragColor;

            // Texture coordinates
            vec2 texcoord0 = texCoordVarying;
			vec2 tc_offset[9];

			void main(void)
			{
				vec3 sample[9];
				vec3 maxValue = vec3(0.0);
		
				tc_offset[0] = vec2(-dilateStep, -dilateStep);
				tc_offset[1] = vec2(-dilateStep, 0);
				tc_offset[2] = vec2(-dilateStep, dilateStep);
				tc_offset[3] = vec2(0, -dilateStep);
				tc_offset[4] = vec2(0, 0);
				tc_offset[5] = vec2(0, dilateStep);
				tc_offset[6] = vec2(dilateStep, -dilateStep);
				tc_offset[7] = vec2(dilateStep, dilateStep);
				tc_offset[8] = vec2(dilateStep, dilateStep);

				for (int i = 0; i < 9; i++)
				{
					sample[i] = texture(baseTex, texcoord0 + tc_offset[i]).rgb;
					maxValue = max(sample[i], maxValue);
				}

				fragColor = vec4(maxValue, 1.0);
			}
		);

		// === Erosion ============================================================================
		erosion = "#version 150\n";
		erosion += STRINGIFY(
			// Input params
			uniform sampler2DRect baseTex;
			uniform int erodeStep;

            in vec2 texCoordVarying;
                             
            out vec4 fragColor;

            // Texture coordinates
            vec2 texcoord0 = texCoordVarying;
			vec2 tc_offset[9];

			void main(void)
			{
				vec3 sample[9];
				vec3 minValue = vec3(1.0);
		
				tc_offset[0] = vec2(-erodeStep, -erodeStep);
				tc_offset[1] = vec2(-erodeStep, 0);
				tc_offset[2] = vec2(-erodeStep, erodeStep);
				tc_offset[3] = vec2(0, -erodeStep);
				tc_offset[4] = vec2(0, 0);
				tc_offset[5] = vec2(0, erodeStep);
				tc_offset[6] = vec2(erodeStep, -erodeStep);
				tc_offset[7] = vec2(erodeStep, erodeStep);
				tc_offset[8] = vec2(erodeStep, erodeStep);

				for (int i = 0; i < 9; i++)
				{
					sample[i] = texture(baseTex, texcoord0 + tc_offset[i]).rgb;
					minValue = min(sample[i], minValue);
				}

				fragColor = vec4(minValue, 1.0);
			}
		);

		// === Final mask ============================================================================
		finalmask = "#version 150\n";
		finalmask += STRINGIFY(
			// Input params
			uniform sampler2DRect baseTex;
			uniform sampler2DRect baseMaskTex;
			uniform sampler2DRect detailMaskTex;
			uniform sampler2DRect inverted_chromaMaskTex;
			uniform vec4 bgColor;
			uniform vec2 endMaskVal;
			uniform float multiplyFilterHueOffset;
			uniform float greenSpillStrength;
			uniform float chromaMaskStrength;
                               
            in vec2 texCoordVarying;
                               
            out vec4 fragColor;

			// Temp. results variables
			vec3 baseMask;
			vec3 detailMask;
			vec3 chromaMask;
			vec3 finalMask;
			vec3 gs_rgb;

			vec3 rgb2hsv(vec3 c)
			{
				vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
				vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
				vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);

				float d = q.x - min(q.w, q.y);
				float e = 1.0e-10;
				return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
			}

			vec3 hsv2rgb(vec3 c)
			{
				vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
				vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
				return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
			}

			void main(){
				float PI = 3.14159265358979323846264;
				vec2 texcoord0 = texCoordVarying;
		
				// Input params
				vec3 image = texture(baseTex, texcoord0).rgb;
				baseMask = texture(baseMaskTex, texcoord0).rgb;
				detailMask = texture(detailMaskTex, texcoord0).rgb;
				vec3 inverted_chromaMask = texture(inverted_chromaMaskTex, texcoord0).rgb;
			
				// Create greenspill-removed input
				vec3 bgColor_hsv = rgb2hsv(bgColor.rgb);
				float bgColor_hue = bgColor_hsv[0];
				vec3 hsv = rgb2hsv(image);
				float hue = hsv[0];	
				float f = 4.0 * greenSpillStrength * sin( 2.0 * PI * ( bgColor_hue + ( multiplyFilterHueOffset - hue )));
				hsv[1] *= max(1.0 - clamp(f, 0.0, 1.0), 0.0);
				gs_rgb = hsv2rgb(hsv);
		
				// Chroma mask
				chromaMask = chromaMaskStrength * (inverted_chromaMask - max(1.0 - vec3(chromaMaskStrength), 0.0));
		
				// Final mask combination
				finalMask = baseMask + detailMask + chromaMask;
				// Clip final mask
				vec3 finalmask_min = vec3(endMaskVal.x);
				vec3 finalmask_max = vec3(endMaskVal.y);
				finalMask = clamp(finalMask, finalmask_min, finalmask_max);
				finalMask = (finalMask - finalmask_min) / (finalmask_max - finalmask_min);
		
				// Final image creation
				fragColor = vec4(gs_rgb, finalMask.r);
			}
		);
	}
};