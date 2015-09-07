//
//  EmitterBar.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 20/06/2015.
//
//

#pragma once

#include "Emitter.h"


class EmitterBar : public Emitter {
    
public:
    
    ofParameter<float> angle;
    ofParameter<float> size;
    
    
    EmitterBar(){
        parameters.add(size.set("size", 10, 10, 1000));
        parameters.add(angle.set("angle", 0, 0, 360));   // emits upwards when angle=0
        size.disableEvents();
        angle.disableEvents();
    }
    
    
    void emit(ParticleData &pd, float dt){
        if(!enabled){
            return;
        }
        
        // with ofMatrix4x4:
        // ofmatrix works like this -> ofVec3f c = a*S*R*T;
//        R = ofMatrix4x4::newRotationMatrix(angle,0,0,1);
//        T = ofMatrix4x4::newTranslationMatrix(position);
//        transform = R*T;
//        
//        ofVec3f emitDirection = ofVec3f(0,-1) * R;
        
        float halfSize = size*0.5;
        
        int maxNewParticles = dt * emitRate;
        int startId = pd.countAlive;
        int endId = min(startId + maxNewParticles, pd.count-1);
        
        for(int i=startId; i<endId; i++){
            Particle& p = pd.particles[i];
            p.reset( ofPoint( ofRandom(-halfSize, halfSize), 1 ) + position );
            
            pd.wake(i);
        }
    }
    
    
    void draw(){
        if(!enabled){
            return;
        }
        float halfSize = size*0.5;
        ofPushMatrix();
        //ofMultMatrix(transform);
        ofTranslate(position);
        ofLine(-halfSize, 0, halfSize, 0);
        ofLine(0, -20, 0, 20);
        ofRect(-10, -10, 20, 20);
        ofDrawBitmapString("emitter\n" + name, 10, 10);
        ofPopMatrix();
    }
    
private:
        
    ofMatrix4x4 R, T;
    ofMatrix4x4 transform;
};
