//
//  EmitterCircle.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 20/06/2015.
//
//

#pragma once

#include "Emitter.h"

class EmitterCircle : public Emitter {
    
public:
    
    ofParameter<float> radiusMin;
    ofParameter<float> radiusMax;
    
    
    EmitterCircle(){
        parameters.add(radiusMin.set("radius min", 10, 10, 1000));
        parameters.add(radiusMax.set("radius max", 10, 10, 1000));
    }
    
    
    void emit(ParticleData &pd, float dt){
        if(!enabled){
            return;
        }
        int maxNewParticles = dt * emitRate;
        int startId = pd.countAlive;
        int endId = min(startId + maxNewParticles, pd.count-1);
        
        for(int i=startId; i<endId; i++){
            Particle& p = pd.particles[i];
            p.reset(position.get() + randomPointInCircle());
            
            pd.wake(i);
        }
    }
    
    
    void draw(){
        if(!enabled){
            return;
        }
        ofPushMatrix();
        ofTranslate(position);
        ofLine(-20, 0, 20, 0);
        ofLine(0, -20, 0, 20);
        ofCircle(0,0, radiusMin);
        ofCircle(0,0, radiusMax);
        ofDrawBitmapString("emitter\n" + name, 10, 10);
        ofPopMatrix();
    }
    
private:
    
    ofPoint randomPointInCircle(){
        ofPoint pnt;
        float radius = ofRandom(radiusMin, radiusMax);
        float angle = ofRandom(0, M_TWO_PI);
        pnt.x = cos(angle) * radius;
        pnt.y = sin(angle) * radius;
        return pnt;
    }
};