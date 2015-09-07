//
//  particleLineRender.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 17/06/2015.
//
//  A view for the particle system
//
//

#pragma once

#include "ParticleData.h"
#include "ofMain.h"


class particleLineRender {
    
public:
    
    particleLineRender();
    
    void setup(ParticleData* pd);
    void update();
    void draw();
    
    ofParameterGroup parameters;
    ofParameter<float> maxLenColor;
    ofParameter<bool> useVelColoring;
    ofParameter<ofFloatColor> color1;
    ofParameter<ofFloatColor> color2;
    ofParameter<float> lineWidth;
    //ofParameter<float> blurScale;
    
private:
    
    ofVboMesh mesh;
    
    ParticleData* pd;
    
    ofFloatColor invisible;
};
