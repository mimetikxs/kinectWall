//
//  PolylineRender.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 12/07/2015.
//
//

#pragma once

#include "ofMain.h"
#include "contourBuffer.h"

class PolylineRender {
    
public:
    
    PolylineRender();
    
    void update(const vector<ofPolyline>& contours, ofPoint center);
    void draw();
    
    ofParameterGroup parameters;
    ofParameter<bool> doInwards;
    ofParameter<float> scaleSpeed;
    ofParameter<int> delay;			// take a snapshoot every delay frames
    ofParameter<bool> useMask;
    ofParameter<bool> useFilled;
    ofParameter<int> colorType;
    ofParameter<bool> enabled;
    ofParameter<float> lineWidth;
    ofParameter<bool> useBuffer;
    ofParameter<int> opacity;
    
private:
    
    ofMesh mask;
    vector<contourBuffer> buffers;
    int bufferSize;
    int index;					// cyclic index
    
    ofTessellator tessellator;
};