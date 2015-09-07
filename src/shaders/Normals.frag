#version 120

uniform sampler2DRect worldPosTexture;		//real world positions (GL_RGB32F_ARB format)
uniform vec2 resolution;					//resolution of the input texture

//const float spread = 2.;					//the higher the more smooth normals are, but problems show at the borders
uniform float spread;

vec3 GetWorld(vec2 xy) {
	xy *= resolution;										//recibe una coordenada normalizada 0.0..1.0, hay que pasarla a 0..640 
	return texture2DRect(worldPosTexture, xy).xyz;
}


void main() {
	vec2 normTextCoord = gl_TexCoord[0].xy / resolution;	//normalized texture coordinate
	vec3 v[4];
	vec2 x  = normTextCoord;
	vec2 dx = spread * .5 / resolution * vec2(1., 0.);		//deltas
	vec2 dy = spread * .5 / resolution * vec2(0., 1.);
	v[0] = GetWorld(x-dx-dy);								//south-west
	v[1] = GetWorld(x+dx-dy);								//south-east
	v[2] = GetWorld(x-dx+dy);								//north-west
	v[3] = GetWorld(x+dx+dy);								//north-east
	
	vec3 r[2];
	r[0] = cross(v[2] - v[0], v[1] - v[0]);					//face1 normal
	r[1] = cross(v[3] - v[2], v[1] - v[3]);					//face2 normal
    
	vec4 col = vec4(1.);
	col.rgb = normalize(r[0] + r[1]);						//add normals and normalize
    
	gl_FragColor = col;	
}