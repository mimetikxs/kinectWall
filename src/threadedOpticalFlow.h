//
//  threadedOpticalFlow.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 08/06/2015.
//
//

#pragma once

#include "ofPixels.h"
#include "ofThread.h"
#include "ofxCv.h"


class threadedOpticalFlow: public ofThread  {
    
public:
    
    threadedOpticalFlow();
    ~threadedOpticalFlow();
    
    void calculateFlow(const ofPixels& pixels); // feed a new frame
    void update();                              // update texture
    //void draw(int x, int y);
        
    ofFloatPixels& getFlowPixelsRef();
    
    ofParameterGroup   parameters;
    ofParameter<float> pyrScale;
    ofParameter<int>   levels;
    ofParameter<int>   winsize;
    ofParameter<int>   iterations;
    ofParameter<int>   polyN;
    ofParameter<float> polySigma;
    ofParameter<bool>  useFarnelGaussian;
    ofParameter<int>   blurSize;
    
protected:
    
    void threadedFunction();
    
    void allocateBuffers(int w, int h);
    
    // incoming pixels
    ofPixels pixelsInBack;
    ofPixels pixelsIn;
    ofPixels pixelsInBlurred;
    // flow result
    ofFloatPixels pixelsFlow;
    //ofTexture textureFlow;

    ofxCv::FlowFarneback flow;
    
    bool bFlowDirty;
    bool bNewPixels;
    
    int height, width; // current size
    
    // parameter listeners
    void pyrScaleChange(float& val){ flow.setPyramidScale(val); }
    void levelsChange(int& val){ flow.setNumLevels(val); }
    void winsizeChange(int& val){ flow.setWindowSize(val); }
    void iterationsChange(int& val){ flow.setNumIterations(val); }
    void polyNChange(int& val){ flow.setPolyN(val); }
    void polySigmaChange(float& val){ flow.setPolySigma(val); }
    void useFarnelGaussianChange(bool& val){ flow.setUseGaussian(val); }
    
    Poco::Condition condition;
};