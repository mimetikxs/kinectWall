//
//  ParticleData.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 18/06/2015.
//
//

#pragma once

#include "Particle.h"

class ParticleData {
    
public:
    
    vector<Particle> particles;
    int count;
    int countAlive;
    
    
    ParticleData()
    :count(0)
    ,countAlive(0){
    }
    
    
    void generate(int maxCount){
        count = maxCount;
//        particles.clear();
//        for(int i=0; i<count; i++) {
//            Particle p;
//            particles.push_back(p);
//        }
        particles.resize(maxCount);
        for(int i=0; i<count; i++) {
            Particle p;
            particles[i] = p;
        }
    }
    
    
    void kill(int i){
        particles[i].isAlive = false;
        swap(particles[i], particles[countAlive-1]);
        countAlive--;
    }
    
    
    void wake(int i){
        particles[i].isAlive = true;
        swap(particles[i], particles[countAlive]);
        countAlive++;
    }
};


