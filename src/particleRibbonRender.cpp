//
//  particleRibbonRender.cpp
//  kinectOpticalFlow
//
//  Created by Nestor Rubio Garcia on 06/07/2015.
//
//

#include "particleRibbonRender.h"

particleRibbonRender::particleRibbonRender(){
    ofFloatColor colorMinValues(0,0,0);
    ofFloatColor colorMaxValues(1,1,1);
    
    parameters.setName("rendering");
    parameters.add(maxLenColor.set("vel color limit", 1, 0, 400));
    parameters.add(color1.set("color 1", ofFloatColor(1,0,.1), colorMinValues, colorMaxValues));
    parameters.add(color2.set("color 2", ofFloatColor(0.1,.3,1), colorMinValues, colorMaxValues));
}


void particleRibbonRender::setup(ParticleData* _pd){
    pd = _pd;
    
    mesh.clear();
    mesh.setUsage(GL_DYNAMIC_DRAW);
    mesh.disableNormals();
    mesh.disableTextures();
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    
    int count = pd->count;
    for(int i=0; i<count; i++) {
        const Particle& p = pd->particles[i];
        
        int trailSize = p.trailSize;
        int iHead = i * trailSize;
        
        for(int j=0; j<trailSize; j++){
            mesh.addVertex(p.trail[j]);
            mesh.addColor(ofFloatColor::white);
            mesh.addIndex(iHead + j);
        }
        
    }
}


void particleRibbonRender::update(){
    float maxLenColorSquared = maxLenColor * maxLenColor;
    
    int firstId = 0;
    int lastId = pd->countAlive;
    
    for(int i=firstId; i<lastId; i++){
        const Particle& p = pd->particles[i];
        
        //float pct = p.vel.lengthSquared() / maxLenColorSquared;
        float pct = p.elapsedLife / p.maxLife;
        ofFloatColor color(color1);
        color.lerp(color2, pct);
        color.a = 1 - pct;
        //color.a = 1 - (p.elapsedLife / p.maxLife);
        
        int trailSize = p.trailSize;
        int iHead = i * trailSize;
        
        // the first vertex (head)
        mesh.setVertex(iHead, p.trail[0]);
        mesh.setColor(iHead, ofFloatColor(0,0));
        
        // the middle vertices
        for(int j=1; j<trailSize-1; j++){
            mesh.setVertex(iHead + j, p.trail[j]);
            mesh.setColor(iHead + j, color);
        }
        
        // the las vertex (tail)
        mesh.setVertex(iHead + trailSize-1, p.trail[trailSize-1]);
        mesh.setColor(iHead + trailSize-1, ofFloatColor(0,0));
    }
    
    // we need to hide the dead particles
    firstId = pd->countAlive;
    lastId = pd->count;
    for(int i=firstId; i<lastId; i++){
        const Particle& p = pd->particles[i];
        int iHead = i * p.trailSize;
        for(int j=0; j<p.trailSize; j++){
            mesh.setColor(iHead + j, ofFloatColor(0,0));
        }
    }
}


void particleRibbonRender::draw(){
    mesh.draw();
}