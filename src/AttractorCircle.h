//
//  AttractorCircleCircle.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 02/07/2015.
//
//

#pragma once

#include "Attractor.h"

class AttractorCircle : public Attractor{

public:
    
    ofParameter<float> spinning;
    ofParameter<bool> bReverseSpinning;
    
    AttractorCircle(){
        parameters.add(spinning.set("spinning", 0, 0, 40));
        parameters.add(bReverseSpinning.set("reverse spinning", false));
    }
    
    
    void attract(ParticleData& pd){
        if(!enabled){
            return;
        }
        float nearDistSq = nearDistance * nearDistance;
        float farDistSq = farDistance * farDistance;
        
        // testing spinning
        float signedSpinning = bReverseSpinning ? spinning.get() : -spinning.get();
        
        for(int i=0; i<pd.countAlive; i++){
            Particle& p = pd.particles[i];
            
            ofVec2f delta = p.pos - position;
            float distSq = delta.lengthSquared();
            
            if(ignoreOutliers){
                if(distSq > farDistSq){
                    continue;
                }
                if(distSq < nearDistSq){
                    p.reset(p.pos);
                    p.isAlive = false;
                    continue;
                }
            }
            
            distSq = ofClamp(distSq, nearDistSq, farDistSq);
            
            delta.normalize();
            float forceStrength = (strength * mass * p.mass) / (distSq);
            
            //testing spinning
            ofVec2f normalLeft(-delta.y, delta.x);
            
            p.applyForce(delta.scale(-forceStrength));
            
            // testing spinning
            p.applyForce(normalLeft.scale(signedSpinning));
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
        ofCircle(0,0, nearDistance);
        ofCircle(0,0, farDistance);
        ofDrawBitmapString("attractor\n" + name, 10, 10);
        ofPopMatrix();
    }
};