//
//  PolylineRender.cpp
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 12/07/2015.
//
//

#include "PolylineRender.h"


PolylineRender::PolylineRender(){
    parameters.setName("poluline render");
    parameters.add( enabled.set("enabled", true) );
    parameters.add( doInwards.set("inwards", true) );
    parameters.add( scaleSpeed.set("scale speed", 0.05, 0.0, 0.5) );
    parameters.add( delay.set("delay", 1, 1.0, 10.0) );
    parameters.add( useMask.set("use mask", false) );
    parameters.add( useBuffer.set("use buffer", true) );
    parameters.add( useFilled.set("use filed", false) );
    parameters.add( opacity.set("opacity", 0, 255, 0));
    parameters.add( lineWidth.set("line width", 1, 1, 5));
    
    // Color type"
    // 0 - b/w when filled, black fill and white stroke otherwise
    // 1 - b/w
    // 2 - gradient
    // 3 - b/w + gradient
    parameters.add( colorType.set("color type", 1, 0, 3) );
    
    
    // mesh buffers
    bufferSize = 20;
    for (int i = 0; i < bufferSize; i++) {
        contourBuffer buffer;
        buffers.push_back( buffer );
    }
    
    index = 0;
}


void PolylineRender::update(const vector<ofPolyline>& contours, ofPoint center){
    if(!enabled){
        return;
    }
    
    // store in buffer
    if(ofGetFrameNum() % delay == 0){
        float sign = (doInwards)? -1 : 1;
        
        
        buffers[index].clearMeshes();
        buffers[index].clearPolylines();
        
        for(int i=0; i<contours.size(); i++){
            const ofPolyline& contour = contours[i];
            
            // tesselate contour
            ofMesh mesh;
            tessellator.tessellateToMesh(contour, OF_POLY_WINDING_NONZERO, mesh, true);	// true = 2D space
            
            // add mesh to buffer (this copies)
            buffers[index].addMesh(mesh);
            buffers[index].addPolyline(contour);
            
            // copy the mesh to mask the strobe
            if(useMask){
                mask = mesh;
            }
        }
        
        // update current buffer
        buffers[index].setCenter( center );
        buffers[index].scale = 1.0;
        buffers[index].scaleSpeed = scaleSpeed * sign;
        buffers[index].age = 0;
        
        // after a snapshoot, move index to the next buffer in the array
        if(index < buffers.size()-1){
            index++;
        }else{
            index = 0;
        }
    }

    // grow buffers
    for(int i = 0; i < buffers.size(); i++){
        buffers[i].grow();
    }
}


void PolylineRender::draw(){
    if(!enabled){
        return;
    }
    
    int buffersDrawed = 0;
    int currBuffer = index;
    
    ofPushStyle();
    
    ofSetLineWidth(lineWidth);
    
    if(useBuffer){
        // draw buffer ordered
        while (buffersDrawed < buffers.size())
        {
            
            ofColor color;
            switch (colorType) {
            case 0:{
                    if(!useFilled){
                        color = ofColor(255);
                        break;
                    }
                }
            case 1:{
                    color = (currBuffer % 2 == 0) ? ofColor::black : ofColor::white;
                    //color = (buffersDrawed % 2 == 0) ? ofColor::black : ofColor::white;	// note that no mask is needed in this mode
                }
                break;
            case 2:{
                    float brightness = 255 - (float(buffers[currBuffer].age) / float(bufferSize)) * 255;
                    color.set(brightness, 255);
                }
                break;
            case 3:{
                    float visible = (currBuffer % 2 == 0) ? 0.0 : 1.0;
                    float brightness = 255 - (float(buffers[currBuffer].age) / float(bufferSize)) * 255;
                    color.set(brightness * visible, 255);
                }
                break;
            }
            
            if(useFilled){
                buffers[currBuffer].colorFill.set(color);
                buffers[currBuffer].colorStroke.set(color);
            }else{
                buffers[currBuffer].colorFill.set(0);
                buffers[currBuffer].colorStroke.set(color);
            }
            
            buffers[currBuffer].draw( OF_MESH_FILL );
            
            // update pointer
            if(doInwards){
                if(currBuffer > 0){
                    currBuffer--;
                }else{
                    currBuffer = buffers.size()-1;
                }
            }else{
                if(currBuffer < buffers.size()-1){
                    currBuffer++;
                }else{
                    currBuffer = 0;
                }
            }
            
            buffersDrawed++;
        }
    }
    
    // mask avoids strobe
    if (useMask) {
        ofSetColor(255);
        mask.drawFaces();
    }
    
    ofPopStyle();
}