//
//  particleSystem.cpp
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 25/03/2015.
//
//

#include "particleSystem.h"

particleSystem:: particleSystem() {
    parameters.setName("physics environment");
    parameters.add(enabled.set("enabled", true));
    parameters.add(stepSize.set("stepSize", 1, 0, 1));
    parameters.add(gravity.set("gravity", -0.01, -3.0, 0.0));
//    parameters.add(dragMin.set("drag min", 0.999, 0.5, 1.0));
//    parameters.add(dragMax.set("drag max", 0.999, 0.5, 1.0));
//    parameters.add(lifeMin.set("life min", 100, 1, 1000));
//    parameters.add(lifeMax.set("life max", 100, 1, 1000));
//    parameters.add(massMin.set("mass min", 1, 0.1, 20));
//    parameters.add(massMax.set("mass max", 1, 0.1, 20));
    
    parameters.add(drag.set("drag", 0.999, 0.5, 1.0));
    parameters.add(life.set("life", 100, 1, 1000));
    parameters.add(mass.set("mass", 1, 0.1, 20));
}


void particleSystem::setup(ofRectangle _bounds) {
    bounds = _bounds;
    
    particledata.generate(MAX_NUM_PARTICLES);
}


void particleSystem::addForceField(forceField* _field){
    field = _field;
}


void particleSystem::addEmitter(Emitter *emitter){
    emitters.push_back(emitter);
}

void particleSystem::addAttractor(Attractor *attractor){
    attractors.push_back(attractor);
}

void particleSystem::update() {
    if(!enabled){
        return;
    }
    
    ParticleData& pd = particledata;
    
    float dt = 30. * ofGetLastFrameTime();  //ofGetTargetFrameRate() / ofGetFrameRate();
    
    dt *= stepSize;
    
    // emitt particles (awake):
    for(int i=0; i<emitters.size(); i++){
        emitters[i]->emit(pd, dt);
    }
    
    // apply forces:
    for(int i=0; i<attractors.size(); i++){
        attractors[i]->attract(pd);
    }
    
    ofVec2f gForce(0,gravity);
    
    // update particles:
    for(int i=0; i<pd.countAlive; i++){
        Particle& p = pd.particles[i];
        
        // apply environment parameters
        p.drag = drag;
        p.mass = mass;
        p.maxLife = life;

        // apply forces
        p.applyForce( gForce );
        p.applyForce( field->getForceAtPos(p.pos) );
        // euler
        p.update(dt);
        
        // kill particles was here
        // extrange flickering... not a good idea to kill while updating
    }
    
    // kill particles (age, bounds, etc):
    float top    = bounds.y;
    float left   = bounds.x;
    float bottom = top + bounds.height;
    float right  = left + bounds.width;
    
    for(int i=0; i<pd.countAlive; i++){
        Particle& p = pd.particles[i];
        if(!p.isAlive
           || p.pos.x < left
           || p.pos.x > right
           || p.pos.y < top
           || p.pos.y > bottom){
            pd.kill(i);
        }
    }
}


void particleSystem::dumpPositions(){
    ParticleData& pd = particledata;
    int size = pd.count;
    for(int i=0; i<size; i++){
        const ofPoint& pos = pd.particles[i].pos;
        //if (isnan(pos.x) || isnan(pos.y))
            cout << pos << endl;
    }
}
