//
//#include "ofApp.h"
//#include "ofMain.h"
//#include "ofAppGlutWindow.h"
//
////========================================================================
//int main( ){
//
//    ofAppGlutWindow window;
//	ofSetupOpenGL(&window, 640*2,480*2, OF_WINDOW);			// <-------- setup the GL context
//
//	// this kicks off the running of my app
//	// can be OF_WINDOW or OF_FULLSCREEN
//	// pass in width and height too:
//	ofRunApp(new ofApp());
//
//}

#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//--------------------------------------------------------------
int main(){
    // see: http://forum.openframeworks.cc/t/dual-monitor-full-screen/13654
    
    ofAppGLFWWindow win;
    //win.setNumSamples(8);
    //this makes the fullscreen window span across all your monitors
    win.setMultiDisplayFullscreen(true);
    
    ofSetupOpenGL(&win, 640*2,480*2, OF_FULLSCREEN);
    ofRunApp(new ofApp()); // start the app
}

