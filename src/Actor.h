//
//  physicsActor.h
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 04/07/2015.
//
//

#pragma once

#include "ofMain.h"
#include "Countable.h"

class Actor : public Countable<Actor> {
    
public:
    
    ofParameterGroup parameters;
    ofParameter<ofPoint> position;
    ofParameter<bool> enabled;
    
    Actor(){
        // default name/id
        name = ofToString(Actor::count(), 3, '0');
        parameters.add(enabled.set("enabled", true));
    }
    
    void setName(string _name){
        name = _name;
        parameters.setName(actorType + " " + name);
        
        // need to have a **unique** name for serialization
        position.setName("position " + actorType + " " + name);
    }
    
    virtual void draw() = 0;
    
protected:
    
    string name;
    string actorType;
};