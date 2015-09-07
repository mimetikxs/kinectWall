//
//  Attractor.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 22/06/2015.
//
//

#pragma once

#include "ofMain.h"
#include "ParticleData.h"
#include "Actor.h"


class Attractor : public Actor {
    
public:
    
    ofParameter<float> strength;
    ofParameter<float> mass;
    ofParameter<float> nearDistance;
    ofParameter<float> farDistance;
    ofParameter<bool> ignoreOutliers;
    
    Attractor(){
        actorType = "attractor";
        setName(name);
        
        //parameters.add(position.set("position", ofPoint(0,0)));
        parameters.add(strength.set("strength", 0.4, -100.0, 100.0));
        parameters.add(mass.set("mass", 1.0, 0.1, 1000.0));
        parameters.add(nearDistance.set("near dist", 50.0, 0.0, 500.0));
        parameters.add(farDistance.set("far dist", 600.0, 0.0, 2000.0));
        parameters.add(ignoreOutliers.set("ignore outliers", true));
    }
    
    virtual void attract(ParticleData& pd) = 0;
};