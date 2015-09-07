//
//  particleEmitter.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 07/04/2015.
//
//

#pragma once

#include "ofMain.h"
#include "Actor.h"
#include "ParticleData.h"

class Emitter : public Actor {
    
public:
    
    ofParameter<float> emitRate;
    
    Emitter(){
        actorType = "emitter";
        setName(name);
        parameters.add(emitRate.set("emit rate", 200, 0, 600));
    }
    
    virtual void emit(ParticleData & pd, float dt) = 0;
};