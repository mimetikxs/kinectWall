/*
 * Treat pixels as vertices.
 * Reproject vertices using projector calibration.
 * The reprojected vertices will be used to draw the blob!
 */

#version 120

uniform sampler2DRect worldPosTexture;
uniform vec2 resolution;

uniform mat4 clipMatrix;					//usamos esta matriz para clipping y efectos
uniform float topDist;
uniform float bottomDist;
uniform float leftDist;
uniform float rightDist;
uniform float nearDist;
uniform float farDist;

const float maxjump = .2;  //0.05			//la diferencia maxima entre este vertex y sus vecinos

varying float existence;					//needed to discard vertices in frag shader



//-------------------------
// find wheter vertex is 'noisy' or out of the allowed distance range
bool jumps (vec2 TexCoord){
	vec2 TextCoordN = TexCoord / resolution;	//normlize texCoord! 
	vec3 wordPos = texture2DRect(worldPosTexture, TexCoord).xyz;
	
	//clip
	vec4 clipPos = clipMatrix * vec4(wordPos, 1.);	// position relative to clipMatrix
	if (clipPos.z < nearDist || clipPos.z > farDist
	|| clipPos.y < bottomDist || clipPos.y > topDist
	|| clipPos.x < leftDist || clipPos.x > rightDist){
		return true;
	}
	
	bool o    = false;
	vec2 dv   = 1. / resolution;
	int steps = 2; //5;
	float r   = length(wordPos);		//note that lookup is done using texture2DRect (not normalized)
	float r2;
	
	//check jump
	for (float x = -dv.x * steps + TextCoordN.x; x <= dv.x * steps + TextCoordN.x; x+= dv.x){
		for (float y = -dv.y * steps + TextCoordN.y; y <= dv.y * steps + TextCoordN.y; y+= dv.y){
			r2 = length(texture2DRect(worldPosTexture, resolution * vec2(x, y)).xyz);
			//if (r - r2 > maxjump || r2 <= nearDist || r2 >= farDist)
			if (r - r2 > maxjump)
				o = true;
		}
	}
	return o;
}


//-------------------------

void main(){
	vec3 worldPos = texture2DRect(worldPosTexture, gl_MultiTexCoord0.xy).xyz;
	
	vec4 pos  = gl_ModelViewProjectionMatrix * vec4(worldPos, 1);	//pos = clip space coordinates
	
	bool zero = jumps(gl_MultiTexCoord0.xy);
	pos.z     = zero ? 100. : pos.z;
	existence = zero ? 0. : 1.;
		
	gl_Position    = pos;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}