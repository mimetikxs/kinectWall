#version 120

const float drop = .9;
const vec4 on = vec4(1.);
const vec4 off = vec4(0.);

varying float existence;


void main() {
    gl_FragColor = (existence > drop) ? on : off;
}
