//
//  AttractorBar.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 01/07/2015.
//
//

#pragma once

#include "Attractor.h"

class AttractorBar : public Attractor {
    
public:
    
    ofParameter<float> angle;
    ofParameter<float> size;
    
    
    AttractorBar(){
        parameters.add(size.set("size", 10, 10, 1000));
        parameters.add(angle.set("angle", 0, 0, 360));   //angle = 0 emits upwards
    }
    
    
    void attract(ParticleData& pd){
        if(!enabled){
            return;
        }
        R = ofMatrix4x4::newRotationMatrix(angle,0,0,1);
        T = ofMatrix4x4::newTranslationMatrix(position);
        transform = R*T;
        
        ofVec3f emitDirection = ofVec3f(0,-1) * R;
        ofVec3f emitDirectionDown = ofVec3f(0,1) * R;
        ofVec3f dir;
        
        float halfSize = size*0.5;
        
        for(int i=0; i<pd.countAlive; i++){
            Particle& p = pd.particles[i];
            
            // bring back the particle to rotated space
            ofPoint unRotated = p.pos * transform.getInverse();
            
            //ignore outliers
            if(unRotated.x < -halfSize || unRotated.x > halfSize){
                continue;
            }
            
            //check the side
            if(unRotated.y >= 0){
                dir = emitDirectionDown;
            }
            else if(unRotated.y < 0){
                dir = emitDirection;
                //p.reset(p.pos);
                //p.isAlive = false;
                //continue;
            }
            else{
                continue;
            }
            
            float distance = ABS(unRotated.y);
            
            if(ignoreOutliers){
                if(distance > farDistance){
                    continue;
                }
                if(distance < nearDistance){
                    p.isAlive = false;
                    continue;
                }
            }
            
            distance = ofClamp(distance, nearDistance, farDistance);
                
            float forceStrength = (strength * mass * p.mass) / (distance * distance);
            
            p.applyForce(dir.scale(forceStrength));
        }
    }
    
    
    void draw(){
        if(!enabled){
            return;
        }
        float halfSize = size*0.5;
        ofPushMatrix();
        ofMultMatrix(transform);
        ofLine(-halfSize, 0, halfSize, 0);
        ofLine(0, 0, 0, farDistance);
        ofLine(-20, farDistance, 20, farDistance);
        ofLine(-10, nearDistance, 10, nearDistance);
        ofDrawBitmapString("attractor\n" + name, 10, 10);
        ofPopMatrix();
    }
    
private:
    
    ofMatrix4x4 R, T;
    ofMatrix4x4 transform;
};