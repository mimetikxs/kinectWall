#version 120


uniform sampler2DRect normalsTexture;
uniform sampler2DRect rgbTexture;

const float   drop = .9;
varying float existence;

varying vec3 position;			//to do: choose better name

uniform int   textureMode;
uniform vec4  lightDirection;	// = vec4(0., -1.5, 0., 0.);
uniform vec4  color1;
uniform vec4  color2;
uniform int   stripesAxis;
uniform float stripesSpeed;
uniform float stripesScale;
uniform float elapsedTime;


void main() {
	
	//Plain color
	if (textureMode == 0) {
		vec4 col = color1;
		col.a = (existence > drop) ? col.a : 0.;		
		gl_FragColor = col;
	}
	
	//Directional light
	else if (textureMode == 1) {		
		vec4 col = vec4( dot(lightDirection, texture2DRect(normalsTexture, gl_TexCoord[0].st)) );
		col.a = (existence > drop) ? 1. : 0.;
		gl_FragColor = col;
	}
	
	//Fast rising stripes
	else if (textureMode == 2) {
		vec4 col = 
			(mod((-position[stripesAxis]) + (elapsedTime * stripesSpeed), stripesScale) < (stripesScale / 2.)) ?
			color1 : color2;	
		gl_FragColor = col;
	}
	
	//Normals texture
	else if (textureMode == 3) {
		vec4 col = texture2DRect(normalsTexture, gl_TexCoord[0].st);
		col.a = (existence > drop) ? 1. : 0.;	// if we use alpha blending we can avoid fragment to be renderes like this
		gl_FragColor = col;
	}
    
    //color texture
    else if (textureMode == 4) {
        vec4 col = texture2DRect(rgbTexture, gl_TexCoord[0].st);
        col.a = (existence > drop) ? 1. : 0.;	// if we use alpha blending we can avoid fragment to be renderes like this
        gl_FragColor = col;
    }
}
