//
//  KinectMeshData.cpp
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 29/06/2015.
//
//

#include "KinectMeshData.h"


KinectMeshData::KinectMeshData(){
}


void KinectMeshData::setup(ofxOpenNI *k){
    kinect = k;
    
    step.set("step", 1, 1, 8);
    roi.set("roi", ofRectangle(0, 0, kinect->getWidth(), kinect->getHeight()));
    
    // these params realocate so we listen for changes
    step.addListener(this, &KinectMeshData::onStepChange);
    roi.addListener(this, &KinectMeshData::onRoiChange);
    
    parameters.setName("kinect data");
    parameters.add(step);
    parameters.add(roi);
    
    generateBuffers();
}


void KinectMeshData::generateBuffers(){
    cols = ceil( roi->width / step );
    rows = ceil( roi->height / step );
    numPixels = cols * rows;
    
    xyzP.allocate(cols, rows, OF_PIXELS_RGB);
    xyzW.allocate(cols, rows, OF_PIXELS_RGB);
    worldPosTexture.allocate(cols, rows, GL_RGB32F);
    
    // projective coords
    ofVec3f *P = (ofVec3f*) xyzP.getPixels();
    int left   = roi->x;
    int top    = roi->y;
    int right  = roi->x + roi->width;
    int bottom = roi->y + roi->height;
    for(int y = top; y < bottom; y += step){
        for(int x = left; x < right; x += step){
            P->x = x;
            P->y = y;
            P->z = 0;
            ++P;
        }
    }
    
    // mesh
    mesh.clear();
    for(int y = 0; y < rows; y++){
        for(int x = 0; x < cols; x++){
            mesh.addVertex(ofVec3f(x, y, 0));
            mesh.addTexCoord(ofVec2f(x, y));
        }
    }
    for(int y = 0; y < rows - 1; y++){
        for(int x = 0; x < cols - 1; x++){
            mesh.addIndex(x+y*cols);				// 00
            mesh.addIndex((x+1)+y*cols);			// 10
            mesh.addIndex(x+(y+1)*cols);			// 01
            mesh.addIndex((x+1)+y*cols);			// 10
            mesh.addIndex((x+1)+(y+1)*cols);		// 11
            mesh.addIndex(x+(y+1)*cols);			// 01
        }
    }
    
    bBuffersDirty = false;
}


void KinectMeshData::update(){
    if(bBuffersDirty) {
        generateBuffers();
    }
    
    // calculate real world coordinates
    XnDepthPixel* pDepthPixels = kinect->getDepthRawPixels().getPixels();
    float *pz  = ((float*)xyzP.getPixels()) + 2;							// pointer to the first projective z-coordinate
    int left   = roi->x;													// left,top,right,bottom indices in the depth map
    int top    = roi->y;
    int right  = roi->x + roi->width;
    int bottom = roi->y + roi->height;
    int i;																	// the current index in the depth map
    
    for(int y=top; y<bottom; y+=step) {                                     // walk through the crop area
        for(int x=left; x<right; x+=step) {
            i   = x + y * KINECT_W;
            *pz = float(pDepthPixels[i]) / 1000.f;							// depth pixel value is distance in mm but we want meters
            pz += 3;														// move pointer to next z value
        }
    }
    
    // coordinates returned are *meters*. the less points we feed in the better performance
    kinect->getDepthGenerator().ConvertProjectiveToRealWorld(numPixels, (XnPoint3D*)xyzP.getPixels(), (XnPoint3D*)xyzW.getPixels());
    
    // upload world positions data to GPU
    worldPosTexture.loadData(xyzW);
}


ofTexture & KinectMeshData::getWorldPositions(){
    return worldPosTexture;
}


ofVboMesh & KinectMeshData::getMesh(){
    return mesh;
}


int KinectMeshData::getWidth(){
    return cols;
}


int KinectMeshData::getHeight(){
    return rows;
}


void KinectMeshData::onStepChange(int& step){
    bBuffersDirty = true;
};


void KinectMeshData::onRoiChange(ofRectangle& roi) {
    bBuffersDirty = true;
};