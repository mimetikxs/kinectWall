//
//  contours.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 13/06/2015.
//
//

#pragma once

#include "ofMain.h"
#include "ofxCv.h"


class contours {
    
public:
    
    contours();
    ~contours();
    
    void findContours(ofPixels& pixels);
    void drawLine();
    void drawFill();
    void drawCenter();
    
    const vector<ofPolyline>& getPolylines() { return polylines; }
    ofPoint getCenter();
    
    ofParameterGroup parameters;
    ofParameter<float> minArea;
    ofParameter<float> simplification;
    ofParameter<float> smoothing;
    ofParameter<float> centerSmoothing;
    
    // render
    ofParameter<int> colorR;
    ofParameter<int> colorG;
    ofParameter<int> colorB;
    ofParameter<int> colorA;
    
protected:
    
    ofxCv::ContourFinder contourFinder;
    vector<ofPolyline> polylines;
    
    ofTessellator tessellator;
    vector<ofMesh> meshes;
    
    ofPoint center;
    ofPoint smoothedCenter;
};
