#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofWindowSettings winSettings;
    winSettings.setSize(1024, 768);
    ofGLWindowSettings glSettings(winSettings);
    glSettings.setGLVersion(3, 2); // Using programmable renderer. Comment out this line to use the 'standard' GL renderer.
    ofCreateWindow(glSettings);
    ofRunApp(new ofApp());
}
