//
//  KinectRender.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 29/06/2015.
//
//

#pragma once

#include "ofMain.h"
#include "KinectMeshData.h"
#include "clippingNode.h"

//#define STRINGIFY(A) #A


class KinectRender {
    
public:
    
    KinectRender(){
    };
    
    void setup(KinectMeshData* _meshData, clippingNode* _clipping){
        meshData = _meshData;
        clipping = _clipping;
        
//        shader.unload();
//        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
//        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
//        shader.linkProgram();
    }
    
    virtual void draw() = 0;
    
    ofParameterGroup parameters;
    
protected:
    
    KinectMeshData * meshData;
    clippingNode * clipping;
    
//    string fragmentShader;
//    string vertexShader;
    ofShader shader;
};