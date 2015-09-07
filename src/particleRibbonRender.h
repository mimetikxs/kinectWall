//
//  particleRibbonRender.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 06/07/2015.
//
//

#pragma once

#include "ParticleData.h"
#include "ofMain.h"


class particleRibbonRender {
    
public:
    
    particleRibbonRender();
    
    void setup(ParticleData* pd);
    void update();
    void draw();
    
    ofParameterGroup parameters;
    ofParameter<float> maxLenColor;
    ofParameter<ofFloatColor> color1;
    ofParameter<ofFloatColor> color2;
    
private:
    
    ofVboMesh mesh;
    
    ParticleData* pd;
    
    ofFloatColor invisible;
};
