//
//  contours.cpp
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 13/06/2015.
//
//

#include "contours.h"

using namespace cv;
using namespace ofxCv;

contours::~contours(){}


contours::contours(){
    parameters.setName("polyline");
    parameters.add( simplification.set("simplification", 1.4, 0.0, 5.0) );
    parameters.add( smoothing.set("smoothing size", 1.5, 0.0, 20.0) );
    parameters.add( minArea.set("min area", 50, 0, 2000) );
    //parameters.add( centerSmoothing.set("center smooth", 0.5, 0.01, 1.0) );
    centerSmoothing.set("center smooth", 0.5, 0.01, 1.0);
    
    // color
    parameters.add( colorR.set("r", 255, 0, 255));
    parameters.add( colorG.set("g", 255, 0, 255));
    parameters.add( colorB.set("b", 255, 0, 255));
    parameters.add( colorA.set("a", 255, 0, 255));
}


void contours::findContours(ofPixels& _pixels){
    // set contourfinder parameters
    contourFinder.findContours(_pixels);
    contourFinder.setMinArea(minArea);
    
    polylines = contourFinder.getPolylines();
    
    meshes.clear();
    
    ofPoint centerAverage;
    
    for(int i=0; i<polylines.size(); i++){
        ofPolyline& polyline = polylines[i];
        
        // simplification and smoothing of contour
        polyline.simplify( simplification );
        ofPolyline smoothed = polyline.getSmoothed(smoothing, 0);	// temp var to store smooth result
        polyline = smoothed;
        
        // tesselate contour
        ofMesh mesh;
        tessellator.tessellateToMesh(polyline, OF_POLY_WINDING_NONZERO, mesh, true);	// true = 2D space
        meshes.push_back(mesh);
        
        // acumulate center
        centerAverage += polyline.getBoundingBox().getCenter();
    }
    
    center = centerAverage / polylines.size();
    smoothedCenter += (center - smoothedCenter) * centerSmoothing;
}

void contours::drawLine(){
    ofPushStyle();
    ofSetColor(255);
    for(int i=0; i<polylines.size(); i++){
        polylines[i].draw();
    }
    ofPopStyle();
}

void contours::drawFill() {
    ofPushStyle();
    ofSetColor(colorR, colorG, colorB, colorA);
    for(int i=0; i<meshes.size(); i++){
        meshes[i].drawFaces();
    }
    ofPopStyle();
}

void contours::drawCenter() {
    ofPushStyle();
    ofSetColor(255);
    
    ofPushMatrix();
    ofTranslate(smoothedCenter.x, smoothedCenter.y, 0);
    ofLine(-20, 0, 20, 0);
    ofLine(0, -20, 0, 20);
    //ofDrawBitmapString("contour \ncenter", 10, 10);
    ofPopMatrix();
    
    ofPopStyle();
}


ofPoint contours::getCenter(){
    //ofScopedLock lock(mutex);
    return smoothedCenter;
}



