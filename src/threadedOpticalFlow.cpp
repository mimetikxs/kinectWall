//
//  threadedOpticalFlow.cpp
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 08/06/2015.
//
//

#include "threadedOpticalFlow.h"


threadedOpticalFlow::~threadedOpticalFlow(){
    stopThread();
    //waitForThread(true);
}

threadedOpticalFlow::threadedOpticalFlow(){
    bFlowDirty = false;
    bNewPixels = false;
    width = 0;
    height = 0;
    
    // init parameters
    parameters.setName("optical flow");
    parameters.add(pyrScale.set("pyrScale", .5, 0, 1));
    parameters.add(levels.set("levels", 4, 1, 8));
    parameters.add(winsize.set("winsize", 8, 4, 64));
    parameters.add(iterations.set("iterations", 2, 1, 8));
    parameters.add(polyN.set("polyN", 7, 5, 10));
    parameters.add(polySigma.set("polySigma", 1.5, 1.1, 2));
    parameters.add(useFarnelGaussian.set("FARNEBACK_GAUSSIAN", false));
    parameters.add(blurSize.set("blur size", 0, 0, 80));
    pyrScale.addListener(this, &threadedOpticalFlow::pyrScaleChange);
    levels.addListener(this, &threadedOpticalFlow::levelsChange);
    winsize.addListener(this, &threadedOpticalFlow::winsizeChange);
    iterations.addListener(this, &threadedOpticalFlow::iterationsChange);
    polyN.addListener(this, &threadedOpticalFlow::polyNChange);
    polySigma.addListener(this, &threadedOpticalFlow::polySigmaChange);
    useFarnelGaussian.addListener(this, &threadedOpticalFlow::useFarnelGaussianChange);
    
    startThread();
}


void threadedOpticalFlow::update(){
    lock();
    if(bNewPixels){
//        textureFlow.loadData(pixelsFlow, GL_RG);
        
        pixelsFlow.allocate(flow.getWidth(), flow.getHeight(), 2);
        memcpy(pixelsFlow.getPixels(), flow.getFlow().ptr<float>(), flow.getWidth() * flow.getHeight() * pixelsFlow.getBytesPerPixel());
        
        //textureFlow.loadData(pixelsFlow, GL_RG);
        
        bNewPixels = false;
    }
    unlock();
}


void threadedOpticalFlow::allocateBuffers(int w, int h) {
    flow.resetFlow();
    
    pixelsInBack.allocate(w, h, 1);
    pixelsIn.allocate(w, h, 1);
    pixelsInBlurred.allocate(w, h, 1);
    
    //pixelsFlow.allocate(w, h, 2);
    //textureFlow.allocate(w, h, GL_RGB32F);
}


void threadedOpticalFlow::calculateFlow(const ofPixels& _pixels){
    lock();
    
    // realocate if size changed since last update
    if(_pixels.getWidth() != width  &&  _pixels.getHeight() != height){
        width = _pixels.getWidth();
        height = _pixels.getHeight();
        allocateBuffers(width, height);
    }
    
    // deep copy of incoming pixels
    // asumes same values in all channels
    //pixelsInBack.setChannel(0, _pixels.getChannel(0));
    pixelsInBack = _pixels.getChannel(0);
    
    bFlowDirty = true;
    
    condition.signal();
    
    unlock();
}


void threadedOpticalFlow::threadedFunction(){
    while(isThreadRunning()){

        lock();
        
        if(!bFlowDirty){
            condition.wait(mutex);
        }
        
        swap(pixelsIn, pixelsInBack);
        // now we can modify pixelsInBack without affecting the calculaton of the flow
        
        unlock();
        
        if(blurSize > 0){
            ofxCv::blur(pixelsIn, pixelsInBlurred, blurSize);
            flow.calcOpticalFlow(pixelsInBlurred);
        }else{
            flow.calcOpticalFlow(pixelsIn);
        }
        
        lock();
        
        // testing...
//        pixelsFlow.allocate(flow.getWidth(), flow.getHeight(), 2);
        //
        
//        memcpy(pixelsFlow.getPixels(), flow.getFlow().ptr<float>(), flow.getWidth() * flow.getHeight() * pixelsFlow.getBytesPerPixel());
        
        bFlowDirty = false;
        bNewPixels = true;
        
        unlock();
    }
}


ofFloatPixels& threadedOpticalFlow::getFlowPixelsRef() {
    ofScopedLock lock(mutex);
    return pixelsFlow;
}


//void threadedOpticalFlow::draw(int x, int y){
//    textureFlow.draw(x, y);
//}
