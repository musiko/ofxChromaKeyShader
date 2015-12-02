#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLWindowSettings settings;
    settings.setGLVersion(3, 2); // Using programmable renderer. Comment out this line to use the 'standard' GL renderer.
    settings.width = 1024;
    settings.height = 768;
    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}
