//
//  KinectRenderBlob.cpp
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 29/06/2015.
//
//

#include "KinectRenderBlob.h"

KinectRenderBlob::KinectRenderBlob(){
    shader.load("shaders/PointCloud.vert", "shaders/PointCloud.frag");
    
    parameters.add(colorR.set("red", 1.0, 0.0, 1.0));
    parameters.add(colorG.set("green", 1.0, 0.0, 1.0));
    parameters.add(colorB.set("blue", 1.0, 0.0, 1.0));
    parameters.add(colorA.set("alpha", 1.0, 0.0, 1.0));
}


void KinectRenderBlob::draw(){
    shader.begin();
    // vertex
    shader.setUniformTexture("worldPosTexture", meshData->getWorldPositions(), 0);
    shader.setUniform2f("resolution", float(meshData->getWidth()), float(meshData->getHeight()));
    // cliping
    shader.setUniformMatrix4f("clipMatrix", clipping->getGlobalTransformMatrix().getInverse().getPtr());
    shader.setUniform1f("nearDist", clipping->near);
    shader.setUniform1f("farDist", clipping->far);
    shader.setUniform1f("topDist", clipping->top);
    shader.setUniform1f("bottomDist", -clipping->bottom);
    shader.setUniform1f("rightDist", clipping->right);
    shader.setUniform1f("leftDist", -clipping->left);
    // fragment
    shader.setUniform4f("colorOn", colorR, colorG, colorB, colorA);
    //shader.setUniformTexture("colorTexture", meshData->getRGBTexture(), 1);
    
    meshData->getMesh().drawFaces();
    
    shader.end();
}