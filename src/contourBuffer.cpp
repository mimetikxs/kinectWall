/*
 *  contourBuffer.cpp
 *
 *  Created by Nestor Rubio Garcia on 01/09/2013.
 *  Copyright 2013 Mimetikxs.com. All rights reserved.
 *
 */

#include "contourBuffer.h"

contourBuffer::contourBuffer(){
	scale = 1.0;
	scaleSpeed = 0.1;
}

void contourBuffer::clearMeshes() {
	meshes.clear();
}

void contourBuffer::addMesh(ofMesh m) {
	meshes.push_back(m);
	
	age = 0;	// reset when a new mesh is added
}

// testing
void contourBuffer::addPolyline(ofPolyline p){
    polylines.push_back(p);
}
void contourBuffer::clearPolylines(){
    polylines.clear();
}

void contourBuffer::setCenter(ofPoint c) {
	translateMatrix.makeTranslationMatrix(c.x, c.y, 0);
}

void contourBuffer::grow() {
	scale += scaleSpeed;
	scaleMatrix.makeScaleMatrix(scale, scale, 0);	
	
	// scale vetices from center:
	// 1. inverse translate vertices (so the origin is placed at the center)
	// 2. scale coords (scaling happens arround the origin)
	// 3. translate back to original position
	transformMatrix = translateMatrix.getInverse() * scaleMatrix * translateMatrix;
	
	age ++;
}


void contourBuffer::draw(ofPolyRenderMode renderMode) {
    ofPushStyle();
	ofPushMatrix();		
	ofMultMatrix(transformMatrix);	
	
    ofSetColor(colorFill);
	for (int i = 0; i<meshes.size(); i++) {
		switch (renderMode) 
		{
			case OF_MESH_POINTS:
				meshes[i].drawVertices();
				break;
			case OF_MESH_WIREFRAME:
				meshes[i].drawWireframe();
				break;
			case OF_MESH_FILL:
				meshes[i].drawFaces();
				break;
		}	
	}
    
    ofSetColor(colorStroke);
    for (int i = 0; i<polylines.size(); i++) {
        polylines[i].draw();
    }
    
	ofPopMatrix();
    ofPopStyle();
}

