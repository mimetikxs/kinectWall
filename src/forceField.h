//
//  forceField.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 25/03/2015.
//
//  A smoothed buffer for the optical flow
//

#pragma once

#include "ofMain.h"
#include "ofxCv.h"

using namespace ofxCv;
using namespace cv;


class forceField {
    
public:
    
    forceField();
    
    void setup(int cols, int rows, float scale = 1);
    
    //void smooth();
    
    //~~~~~~~~~~~~~
    // TODO:
    // get the cell coords so we can reuse it to retrieve
    // forces overlapping :)
    // eg: noise filed and flow field (one cell contains booth forces)
    // ofVec2f getCellCoordAtPosition(ofPoint& position);
    //~~~~~~~~~~~~~~~
    
    
    // TODO:
    // add acumulated forces to the field
    //void update();
    
    // TODO:
    // delete addFlow and setNoise and implement addForce
    //void addForce(const ofFloatPixels& flow);
    void addFlow(const ofFloatPixels& flow);
    void setNoise(const ofFloatPixels& flow);
    
    //void setForceAtCell(int col, int row, const ofVec2f& force);
    
    ofVec2f getForceAtCell(int col, int row);
    ofVec2f getForceAtPos(float x, float y);
    ofVec2f getForceAtPos(const ofPoint& pos);
    
    const ofFloatPixels& getFieldPixels();
    
    ofParameterGroup parameters;
    ofParameter<float> flowSmoothFactor;
    ofParameter<float> minFlow;
    //ofParameter<float> scaleFactor;
    ofParameter<float> flowScaleFactor;
    ofParameter<float> noiseScaleFactor;
    ofParameter<ofVec2f> position;
    
    // debug:
    void drawVectors();
    
private:
    
//    ofFloatPixels fieldNoise;
//    ofFloatPixels fieldFlow;
    
    // TODO: acumulate all the forces
    //ofFloatPixels fieldAcumulator;
    
    ofFloatPixels fieldPix;
    ofTexture fieldTex;
    
    float scale;
    
    int cols;
    int rows;
};