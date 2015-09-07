//
//  particleSystem.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 25/03/2015.
//
//
//

#pragma once

#include "ofMain.h"
#include "ParticleData.h"
#include "forceField.h"
#include "Actor.h"
#include "Attractor.h"
#include "AttractorBar.h"
#include "AttractorCircle.h"
#include "Emitter.h"
#include "EmitterBar.h"
#include "EmitterCircle.h"
#include "EmitterPoints.h"

#define MAX_NUM_PARTICLES 5000 //15000


class particleSystem {
    
public:
    
    particleSystem();
    
    void setup(ofRectangle worldBounds);
    void update();
    
    void addForceField(forceField* field);
    void addEmitter(Emitter* emitter);
    void addAttractor(Attractor* attractor);
    
    const ParticleData& getParticleData() const { return particledata; }
    ParticleData& getParticleData() { return particledata; }
    
    ofParameterGroup parameters;
    ofParameter<bool> enabled;
    ofParameter<float> stepSize;
    ofParameter<float> gravity; // TODO: make direction editable
    ofParameter<float> drag;
    ofParameter<float> life;
    ofParameter<float> mass;
    
    void dumpPositions();
    
private:
    
    ParticleData particledata;
    
    vector<Emitter*> emitters;
    Vector<Attractor*> attractors;
    //vector<forceField *> fields;
    forceField* field;
    
    
    
    ofRectangle bounds;
};