/*
 *  clippingNode.h
 *
 *  Created by Nestor Rubio Garcia on 19/10/2013.
 *  Copyright 2013 Mimetikxs.com. All rights reserved.
 *
 *	A ofNode (3D coord system) that defines a cuboid. It can be thought as an orthographic frustrum.
 *	Its origin is at the center of "viewport".
 *	It caches values such: near, far, top, bottom, left and right
 *
 */


#include "clippingNode.h"


clippingNode::clippingNode(){
    posX.addListener(this, &clippingNode::onPositionChanged);
    posY.addListener(this, &clippingNode::onPositionChanged);
    posZ.addListener(this, &clippingNode::onPositionChanged);
    
    rotX.addListener(this, &clippingNode::onRotationChanged);
    rotY.addListener(this, &clippingNode::onRotationChanged);
    rotZ.addListener(this, &clippingNode::onRotationChanged);
    
    groupWalls.setName("walls");
    groupWalls.add( near.set("near", 0.0, 0.0, 8.0) );
    groupWalls.add( far.set("far", 8.0, 0.0, 8.0) );
    groupWalls.add( left.set("left", 2.0, 0.0, 3.0) );
    groupWalls.add( right.set("right", 2.0 ,0.0, 3.0) );
    groupWalls.add( top.set("top", 2.0, 0.0, 3.0) );
    groupWalls.add( bottom.set("bottom", 2.0, 0.0, 3.0) );
    groupPosition.setName( "position" );
    groupPosition.add( posX.set("pos x", 0.0, -2.0, 2.0) );
    groupPosition.add( posY.set("pos y", 0.0, -2.0, 2.0) );
    groupPosition.add( posZ.set("pos z", 0.0, -2.0, 2.0) );
    groupRotation.setName("rotaion");
    groupRotation.add( rotX.set("rot x", 0.0, -90.0, 90.0) );
    groupRotation.add( rotY.set("rot y", 0.0, -90.0, 90.0) );
    groupRotation.add( rotZ.set("rot z", 0.0, -90.0, 90.0) );
    parameters.setName( "clipping" );
    parameters.add(groupWalls);
    parameters.add(groupPosition);
    parameters.add(groupRotation);
    
    // testing
    // this exposes depth range parameters
    groupDepth.setName("depth");
    groupDepth.add(near);
    groupDepth.add(far);
	
	mesh.setMode(OF_PRIMITIVE_LINES);
}


void clippingNode::updateMesh(){
	mesh.clear();	
	// recreate mesh (from top-left clockwise)	
	mesh.addVertex(ofVec3f(-left, top,	   near));
	mesh.addVertex(ofVec3f(right, top,	   near));
	mesh.addVertex(ofVec3f(right, -bottom, near));
	mesh.addVertex(ofVec3f(-left, -bottom, near));
	mesh.addVertex(ofVec3f(-left, top,	   far));
	mesh.addVertex(ofVec3f(right, top,	   far));
	mesh.addVertex(ofVec3f(right, -bottom, far));
	mesh.addVertex(ofVec3f(-left, -bottom, far));	
	const ofIndexType segments[] = { 0,1, 1,2, 2,3, 3,0, 4,5, 5,6, 6,7, 7,4, 0,4, 1,5, 2,6, 3,7 };
	mesh.addIndices(&segments[0], 24);
}


void clippingNode::onRotationChanged(float& value){
    setOrientation(ofVec3f(rotX, rotY, rotZ));
}


void clippingNode::onPositionChanged(float& value){
    setPosition(posX, posY, posZ);
}


void clippingNode::customDraw(){
    updateMesh();
    //ofNode's transformation
    mesh.drawWireframe();
}