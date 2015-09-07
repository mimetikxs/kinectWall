//
//  Particle.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 20/03/2015.
//
//

#pragma once

#include "ofMain.h"

class Particle {
    
public:
    
    ofPoint pos;
    ofVec2f vel;
    ofVec2f accel;
    float   mass;
    float   drag;
    
    float   maxLife;
    float   elapsedLife;
    bool    isAlive;
    
    // previous positions
//    ofPoint prevPos5;
//    ofPoint prevPos4;
//    ofPoint prevPos3;
//    ofPoint prevPos2;
//    ofPoint prevPos1;
//    ofPoint prevPos;
    int trailSize;
    vector<ofPoint> trail; // newest positions first
    
    
    Particle() {
        mass = 1.0;
        drag = 0.999;
        isAlive = false;
        maxLife = 0;
        elapsedLife = 0;
        
        // note: the first vert of the trail is the same as pos
        trailSize = 2;
        for(int i=0; i<trailSize; i++){
            trail.push_back(ofPoint());
        }
    }
    
    
    void reset(ofVec2f newPos){
        accel.set(0);
        elapsedLife = 0;
        pos.set(newPos);
        vel.set(0);
        
        // trails
        for(int i=0; i<trailSize; i++){
            trail[i].set(pos);
        }
    }
    
    
    //void setup(const ofPoint& pos, const ofVec2f& vel, float _maxLife);
    void applyForce(const ofVec2f& force){
        accel += force / mass;
    }
    
    
    void update(float dt){
        // update trails
//        for(int i=trailSize-1; i>0; i--){
//            trail[i].set( trail[i-1] );
//        }
        trail[1].set(trail[0]);
        trail[0].set(pos);
        
        vel += accel;
        vel *= drag;
        pos += vel * dt;
        accel.set(0);
        
        elapsedLife += dt;
        
        if(elapsedLife > maxLife){
            isAlive = false;
        }
    }
};