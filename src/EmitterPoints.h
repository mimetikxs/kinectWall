//
//  EmitterPoints.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 06/07/2015.
//
//

#pragma once

#include "Emitter.h"


class EmitterPoints : public Emitter {
    
public:
    
    ofParameter<float> randomness;
    
    
    EmitterPoints(){
        parameters.add(randomness.set("randomnes", 0, 0, 1));
    }
    
    
    void clear() {
        points.clear();
    }
    void setPoints(vector<ofPoint>& _points){
        points = _points;
    }
    void setPoint(ofPoint _point){
        points.push_back(_point);
    }
    
    
    void emit(ParticleData &pd, float dt){
        if(!enabled){
            return;
        }
        int maxNewParticles = dt * emitRate;
        int startId = pd.countAlive;
        int endId = min(startId + maxNewParticles, pd.count-1);
        
        if (points.size() == 0) return;
        
        // number of particles to emmit
        for(int i=startId; i<endId; i++){
            Particle& p = pd.particles[i];
            p.reset( getRandomPoint() );
            
            pd.wake(i);
        }
    }
    
    
    void draw(){
        if(!enabled){
            return;
        }
        for(int i=0; i<points.size(); i++){
            ofRect(points[i], 2, 2);
        }
    }
    
private:
    
    vector<ofPoint> points;
    
    
    const ofPoint& getRandomPoint(){
        int index = ofMap(ofRandom(randomness), 0, 1, 0, points.size());
        return points[index];
    }
    
};