#pragma once

#include "ofxCv.h"
#include "ofxOpenNI.h"
#include "ofxOpenNICapture.h"
#include "ofxGui.h"
#include "ofxFluid.h"
#include "ofMain.h"

#include "HomographyHelper.h"
#include "HomographyViewport.h"

#include "particleLineRender.h"
#include "particleRibbonRender.h"
#include "particleSystem.h"

#include "threadedOpticalFlow.h"
#include "contours.h"

#include "GuiPanelLayoutManager.h"
#include "GuiPresets.h"

#include "OscController.h"

//#include "PolylineRender.h"

#include "ofxFastFboReader.h"
#include "ofxTimeMeasurements.h"


// constants

#define KINECT_W 640
#define KINECT_H 480
#define PROJECTOR_W 1280 //1024 1280 1920
#define PROJECTOR_H 800  //768 800 1080

#define PATH_LAYOUT "settings/layout.xml"


class ofApp : public ofBaseApp{
    
public:
    
    ofApp();
    
    void exit();
	void setup();
	void update();
	void draw();
    void drawDebug();
    void drawGui();
    
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
    //~~~~~~~~~~~~~~~~
    // global
    //~~~~~~~~~~~~~~~~
    
    void refreshRoiWraped();
    
    
    //~~~~~~~~~~~~~~~~
    // kinect
    //~~~~~~~~~~~~~~~~
    
    ofxOpenNI kinect;
    
    //~~~~~~~~~~~~~~~~
    // contours
    //~~~~~~~~~~~~~~~~
    
    contours contourFinder;
    
    //~~~~~~~~~~~~~~~~
    // optical flow
    //~~~~~~~~~~~~~~~~
    
    threadedOpticalFlow flow;
    unsigned long long last;
    
    //~~~~~~~~~~~~~~~~
    // physics
    //~~~~~~~~~~~~~~~~
    
    forceField forceField;
    EmitterCircle emitterCircle;
    EmitterBar emitterBar;
    EmitterPoints emitterBlob;
    AttractorCircle attractorCircle;
    AttractorBar attractorBar;
    particleSystem physics;
    particleLineRender physicsRender;
    
    //actor mouse picking
    vector<Actor*> physicsActors;
    Actor* pickedActor;
    
    // debug
    ofTexture texForcefield;
    
    //~~~~~~~~~~~~~~~~
    // layers
    //~~~~~~~~~~~~~~~~
    
    ofFbo fboPhysics;
    ofFbo fboContours;
    
    //~~~~~~~~~~~~~~~~
    // gui
    //~~~~~~~~~~~~~~~~
    
    // input image
    ofxPanel panelInput;
    ofParameterGroup groupRoi;
    ofParameter<float> top;
    ofParameter<float> left;
    ofParameter<float> bottom;
    ofParameter<float> right;
    ofxButton btnApplyRoi;
    void applyRoi();
    ofParameterGroup groupBlobCleaning;
    ofParameter<float> thresMin;
    ofParameter<float> thresMax;
    ofParameter<int> dilateRadius;
    ofParameter<int> erodeRadius;
    
    // optical flow
    ofxPanel panelFlow;
    
    // force filed
    ofxPanel panelForceField;
    
    // contours
    ofxPanel panelContours;
    
    // physics
    ofxPanel panelPhysics;
    ofParameterGroup groupPhysics;
    
    // layer
    ofxPanel panelLayer;
    ofParameter<float> fadeAmnt;
    
    // debug
    ofxPanel panelDebug;
    ofParameter<bool> bDrawDebugMaps;
    ofParameter<bool> bDrawDebugFieldVectors;
    ofParameter<bool> bDrawDebugPhysics;
    ofParameter<bool> bDrawDebugRois;
    ofParameter<bool> bDrawDebugContours;
    
    // panel manager
    ofxPanel panelManager;
    ofParameter<bool> bShowPanelDebug;
    ofParameter<bool> bShowPanelPhysics;
    ofParameter<bool> bShowPanelFlow;
    ofParameter<bool> bShowPanelField;
    ofParameter<bool> bShowPanelContours;
    ofParameter<bool> bShowPanelInput;
    ofParameter<bool> bShowPanelLayer;
    ofParameter<bool> bShowHomography; // this activates/deactivates homography mode
    void onShowHomographyChange(bool& val);
    
    // layout manager
    GuiPanelLayoutManager layout;
    
    bool bDrawGui;
    void buildGui();
    
    //~~~~~~~~~~~~~~~~
    // testing
    //~~~~~~~~~~~~~~~~
    
    bool bAddFlow;
    
    // kienct emmiter
    //vector<ofPoint> blobPoints;    
    void updateBlobPoints(ofPixels pixels, int step = 1, int min = 0, int max = 255);
    
    
    // testing simple kinect blob
    ofPixels pixelsThresh;
    ofTexture texThresh;
    int stepKinect;
    
    
    ofRectangle viewporKinect;      // non-scaled viewports
    ofRectangle viewportProjector;  //
    
    // interactive areas for homography
    HomographyViewport homographySrc;
    HomographyViewport homographyDst;
    
    ofMatrix4x4 homography;
    
    ofVboMesh planeMesh;
    
    
    // rois are in a range 0-1
    ofRectangle roiProjetor;
    bool bRoiDirty;
    
    // this is a down-sampled version of the kinect warped into the projector
    ofFbo fboWarped;
    ofPixels pixelsWraped;
    float wrapedScale;
    
    
    // disable heavy computations
    bool doCv;
    
   
    ofMatrix4x4 transformWarpToProjector;
    
    // presets
    ofParameterGroup groupPhysicsPreset;
    GuiPresets presetsPhysics;
    ofParameterGroup groupContoursPreset;
    GuiPresets presetsContours;
    ofParameterGroup groupFluidPreset;
    GuiPresets presetsFluid;
    
    
    // erosion dilation
    //ofFbo fboDilatedEroded;
    ofTexture texDilatedEroded;
    ofPixels pixDilated;
    ofPixels pixEroded;
    void erode(ofPixels& _src, ofPixels& _dst);
    void dilate(ofPixels& _src, ofPixels& _dst);
    
    
    OscController osc;
    
    
    void saveHomography();
    void loadHomography();
    
    
    // frame differencing
    ofPixels grey;
    ofPixels previous;
    ofImage diff;
    ofFloatPixels floatDiff;
    ofFloatPixels floatBuffer;
    ofTexture texBuffer;
    //ofFbo fboDiffBuffer;
    ofFloatPixels floatDiffThresholded;
    
    ofxPanel panelFrameDiff;
    ofParameter<float> clearPct;
    ofParameter<int> minThresh;
    ofParameter<int> maxThresh;
    
    // fluid
    
    ofxFluid fluid;
    ofFloatPixels pixelsVel;
    ofTexture texVel;
    ofFbo fboColor;
    
    ofFloatPixels test;
    
    ofFbo fboFluidVel;
    ofFloatPixels pixelsFluidVel;
    
    ofxPanel panelFluid;
    ofParameter<float> inkPct;
    ofParameter<float> velPct;
    ofParameter<float> disipation;
    ofParameter<float> velDisipation;
    ofParameter<float> gravity;
    ofParameter<ofColor> fluidColor;
    
    ofxFastFboReader reader;
//    ofxFastFboReader reader2;
};

