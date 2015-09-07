/*
 *  clippingNode.h
 *
 *  Created by Nestor Rubio Garcia on 19/10/2013.
 *  Copyright 2013 Mimetikxs.com. All rights reserved.
 *
 *	A ofNode (3D coord system) that defines a cuboid.
 *	We can get/set the distances (from the origin) to each of the 6 faces: 
 *	near, far, top, bottom, left and right
 *
 */

#pragma once

#include "ofNode.h"
#include "ofMesh.h"
#include "ofParameterGroup.h"
#include "ofParameter.h"


class clippingNode : public ofNode {
    
    public:
    
        clippingNode();
    
        ofParameterGroup groupDepth;
    
        ofParameterGroup groupWalls;
        ofParameter<float> top;
        ofParameter<float> bottom;
        ofParameter<float> left;
        ofParameter<float> right;
        ofParameter<float> near;
        ofParameter<float> far;
    
        ofParameterGroup groupPosition;
        ofParameter<float> posX;
        ofParameter<float> posY;
        ofParameter<float> posZ;
    
        ofParameterGroup groupRotation;
        ofParameter<float> rotX;
        ofParameter<float> rotY;
        ofParameter<float> rotZ;
    
        ofParameterGroup parameters;
    
        void customDraw();
        
    private:
    
        ofMesh mesh;
    
        void updateMesh();
        
        void onPositionChanged(float& value);
        void onRotationChanged(float& value);
};