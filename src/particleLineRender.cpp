//
//  particleLineRender.cpp
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 17/06/2015.
//
//

#include "particleLineRender.h"


particleLineRender::particleLineRender(){
    ofFloatColor colorMinValues(0,0,0);
    ofFloatColor colorMaxValues(1,1,1);
    
    parameters.setName("rendering");
    parameters.add(useVelColoring.set("vel coloring", true));
    parameters.add(maxLenColor.set("vel color limit", 1, 0, 400));
    parameters.add(color1.set("color 1", ofFloatColor(1,0,.1), colorMinValues, colorMaxValues));
    parameters.add(color2.set("color 2", ofFloatColor(0.1,.3,1), colorMinValues, colorMaxValues));
    parameters.add(lineWidth.set("line width", 2, 2, 10));
}


void particleLineRender::setup(ParticleData* _pd){
    pd = _pd;
    
    mesh.clear();
    mesh.setUsage(GL_DYNAMIC_DRAW);
    mesh.disableNormals();
    mesh.disableTextures();
    mesh.setMode(OF_PRIMITIVE_LINES);
    
    int count = pd->count;
    for(int i=0; i<count; i++) {
        int i2 = i * 2;
        const Particle& p = pd->particles[i];
        mesh.addVertex(p.pos);
        mesh.addVertex(p.pos);
        mesh.addColor(ofFloatColor::white);
        mesh.addColor(ofFloatColor::white);
        mesh.addIndex(i2);
        mesh.addIndex(i2 + 1);
    }
}


void particleLineRender::update(){    
    float maxLenColorSquared = maxLenColor * maxLenColor;
    
    int firstId = 0;
    int lastId = pd->countAlive;
    
    for(int i=firstId; i<lastId; i++){
        const Particle& p = pd->particles[i];
        
        // color
        ofFloatColor color(color1);
        float pct = 0;
        
        if(useVelColoring){
            pct = p.vel.lengthSquared() / maxLenColorSquared;
            color.lerp(color2, pct);
            color.a = pct;
        }else{
            pct = p.elapsedLife / p.maxLife;
            color.lerp(color2, pct);
            color.a = 1 - pct;
        }
        
        int i2 = i * 2;
        
        mesh.setVertex(i2,   p.trail[0]);
        mesh.setVertex(i2+1, p.trail[1]);
        mesh.setColor(i2,   color);
        mesh.setColor(i2+1, color);
    }
    
    // we need to hide the dead particles
    firstId = pd->countAlive;
    lastId = pd->count;
    for(int i=firstId; i<lastId; i++){
        int i2 = i * 2;
        mesh.setColor(i2,   ofFloatColor(0,0));
        mesh.setColor(i2+1, ofFloatColor(0,0));
    }
}


void particleLineRender::draw(){
    ofPushStyle();
    ofSetLineWidth(lineWidth);
    mesh.draw();
    ofPopStyle();
}