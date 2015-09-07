//
//  KinectMeshData.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 29/06/2015.
//
//

#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"

#define KINECT_W 640
#define KINECT_H 480

class KinectMeshData {
    
public:
    KinectMeshData();

    void setup(ofxOpenNI* kinect);
    void update();
    
    ofTexture & getWorldPositions();
    ofTexture & getRGBTexture(){ return kinect->getRGBTextureReference(); }
    ofVboMesh & getMesh();
    int getHeight();
    int getWidth();
    
    ofParameter<int> step;
    ofParameter<ofRectangle> roi;
    ofParameterGroup parameters;
    
private:
    
    ofxOpenNI *	kinect;
    
    // this ensures that the textures are realocated without crashing the program.
    // if we were to set ::step or ::crop when the textures are being used it'd crash the program
    bool bBuffersDirty;
    void onStepChange(int& step);
    void onRoiChange(ofRectangle& roi);
    void generateBuffers();
    
    ofFloatPixels xyzP;					//projective coordinates
    ofFloatPixels xyzW;					//real world coordinates
    ofTexture     worldPosTexture;		//stores real world positions
    
    ofVboMesh mesh;
    
    //some handy cached values
    int	cols, rows;						//size of data after applying crop and step
    int	numPixels;						//number of pixels in the texture
};