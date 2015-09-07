/*
 *  contourBuffer.h
 *
 *  Created by Nestor Rubio Garcia on 01/09/2013.
 *  Copyright 2013 Mimetikxs.com. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"


class contourBuffer {

public:
	
	contourBuffer();
	
	void clearMeshes();
	void addMesh(ofMesh m);
	void setCenter(ofPoint c);
    
    //testing
    void clearPolylines();
    void addPolyline(ofPolyline p);
	
	void grow();
	
	void draw(ofPolyRenderMode renderMode);
		
	float scale;
	float scaleSpeed;
	
	// in frames
	int age;
	//int life;
    
    ofColor colorFill;
    ofColor colorStroke;
	
private:
	
	ofMatrix4x4 scaleMatrix;
	ofMatrix4x4 translateMatrix;
	ofMatrix4x4 transformMatrix;
	
	vector<ofMesh> meshes;
    vector<ofPolyline> polylines;
};