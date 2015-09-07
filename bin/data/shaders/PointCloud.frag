#version 120

const float drop = .9;
const vec4 on = vec4(1.);
const vec4 off = vec4(0.);

//uniform sampler2DRect colorTexture;

uniform vec4 colorOn;

varying float existence;

void main(){
    gl_FragColor = (existence > drop) ? colorOn : off;
}
