//
//  KinectRenderBlob.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 29/06/2015.
//
//

#pragma once

#include "KinectRender.h"

class KinectRenderBlob : public KinectRender {
    
public:
    
    KinectRenderBlob();
    
    void draw();
    
    ofParameter<float> colorR;
    ofParameter<float> colorG;
    ofParameter<float> colorB;
    ofParameter<float> colorA;
};