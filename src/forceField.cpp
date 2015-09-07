//
//  forceField.cpp
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 25/03/2015.
//
//

#include "forceField.h"



forceField::forceField(){
    parameters.setName("force field");
    parameters.add(flowSmoothFactor.set("flow smooth", 0.1, 0.0, 1.0));
    parameters.add(minFlow.set("flow min threshold", 0.0, 0.0, 20.0));
    parameters.add(flowScaleFactor.set("flow scale", 1.0, 0.0, 50.0));
    parameters.add(noiseScaleFactor.set("noise scale", 1.0, 0.0, 10.0));
    //parameters.add(scaleFactor.set("stregth", 0.1, 0.0, 100.0));
}


void forceField::setup(int _cols, int _rows, float _scale){
    scale = _scale;
    cols = _cols;
    rows = _rows;
    
    fieldPix.allocate(cols, rows, 2);
//    fieldFlow.allocate(cols, rows, 2);
//    fieldNoise.allocate(cols, rows, 2);
//
//    // testing noise
//    for( int y=0; y<fieldNoise.getHeight(); y++){
//        for( int x=0; x<fieldNoise.getWidth(); x++){
//            float noise = ofMap( ofNoise(x*0.009, y*0.009), 0, 1, 0, TWO_PI);
//            int i = fieldNoise.getPixelIndex(x,y);
//            fieldNoise[i] = cos(noise);
//            fieldNoise[i+1] = sin(noise);
//        }
//    }
}


void forceField::addFlow(const ofFloatPixels& flow){
    if(flow.getWidth() != fieldPix.getWidth() || flow.getHeight() != fieldPix.getHeight()){
        cout << "[ERROR] forceField: field and flow size must match ->";
        cout << " field = " << fieldPix.size();
        cout << " flow = " << flow.size() << endl;
        return;
    }
    
    for(int i=0; i<flow.size(); i++){
        float flowVal = flow[i];
        // when realocating (change of size) we can get NaN values from the flow
        // we need to set the value to a float if we don't wan't this error to spread
        if (isnan(flowVal)) {
            cout << "flow is not a number!" << endl;
            flowVal = 0.f;
        }
        
        fieldPix[i] += (flowVal - fieldPix[i]) * flowSmoothFactor;
    }
    
    /*
    float minFlowSq = minFlow*minFlow;
    
    for(int i=0; i<flow.size(); i+=2){
        float flowValX = flow[i];
        float flowValY = flow[i+1];
        // when realocating (change of size) we can get NaN values from the flow
        // we need to set the value to a float if we don't wan't this error to spread
//        if (isnan(flowValX)) {
//            cout << "flow is not a number!" << endl;
//            flowValX = 0.f;
//        }
//        if (isnan(flowValY)) {
//            cout << "flow is not a number!" << endl;
//            flowValY = 0.f;
//        }
        
        float lenSq = (flowValX*flowValX + flowValY*flowValY);
        if(lenSq < minFlowSq){
            flowValX = 0;
            flowValY = 0;
        }
        
        // add the flow
        fieldFlow[i]   += (flowValX - fieldFlow[i]) * flowSmoothFactor;
        fieldFlow[i+1] += (flowValY - fieldFlow[i+1]) * flowSmoothFactor;
        
        // combine both
        fieldPix[i]   = (fieldNoise[i] * noiseScaleFactor) + (fieldFlow[i] * flowScaleFactor);
        fieldPix[i+1] = (fieldNoise[i+1] * noiseScaleFactor) + (fieldFlow[i+1] * flowScaleFactor);
    }
     */
}


ofVec2f forceField::getForceAtCell(int col, int row){
    int i = fieldPix.getPixelIndex(col, row);
    float x = fieldPix[i];
    float y = fieldPix[i+1];
    
//    if(isnan(x) || isnan(y)){
//        x = 0;
//        y = 0;
//    }
    
    return ofVec2f(x*flowScaleFactor, y*flowScaleFactor);
    
//    return ofVec2f(x, y);
}


ofVec2f forceField::getForceAtPos(const ofPoint& pos) {
    ofPoint localPos( pos - position.get() );
    ofPoint cell( localPos / scale );
    
//    cell.x = ofClamp( cell.x, 0, cols-1 );
//    cell.y = ofClamp( cell.y, 0, rows-1 );

    if(cell.x > 0
       && cell.x < cols-1
       && cell.y > 0
       && cell.y < rows-1){
        return getForceAtCell(cell.x, cell.y);
    }
    
    return ofVec2f(0);
}


ofVec2f forceField::getForceAtPos(float screenX, float screenY){
    return getForceAtPos(ofPoint(screenX, screenY));
}


const ofFloatPixels& forceField::getFieldPixels(){
    return fieldPix;
}


// debug:
void forceField::drawVectors(){
    ofPushStyle();
    ofSetLineWidth(1);
    ofSetColor(255);
    for(int x=0; x<cols; x+=4){
        for(int y=0; y<rows; y+=4){
            ofVec2f tail = ofVec2f(x,y) * scale + position;
            ofVec2f head = tail + getForceAtCell(x,y) * scale;
            ofLine(tail, head);
        }
    }
    ofPopStyle();
}


