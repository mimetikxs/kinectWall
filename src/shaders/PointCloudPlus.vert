#version 120

//uniform sampler2DRect rgbTexture;			//GL_RGBA
uniform sampler2DRect worldPosTexture;		//GL_RGB32F_ARB (rgb channels = xyz components)
uniform sampler2DRect normalsTexture;		//GL_RGB32F_ARB
uniform vec2 resolution;					//resolution of the input texture
//clipping
uniform mat4  clipMatrix;					//this matrix transforms vertices into clip box space
uniform float topDist;
uniform float bottomDist;
uniform float leftDist;
uniform float rightDist;
uniform float nearDist;
uniform float farDist;

//spikes/fat effect
uniform int   spikesStep;
uniform float spikesScale;
//

const float maxjump = .2;  //0.05			//la diferencia maxima entre este vertex y sus vecinos

varying vec3 position;						//needed for the stripes effect in frag shader
varying float existence;					//needed to discard vertices in frag shader



//-------------------------
// find wheter vertex is 'noisy' or out of the allowed distance range (i need to do more research)

bool jumps (vec2 TexCoord) {
	vec2 TextCoordN = TexCoord / resolution;	//normlize texCoord! 
	vec3 wordPos = texture2DRect(worldPosTexture, TexCoord).xyz;
	
	//clip
	vec4 clipPos = clipMatrix * vec4(wordPos, 1.);	// position relative to clipMatrix
	if (clipPos.z < nearDist || clipPos.z > farDist
	|| clipPos.y < bottomDist || clipPos.y > topDist
	|| clipPos.x < leftDist || clipPos.x > rightDist) {
		return true;
	}
	
	bool o    = false;
	vec2 dv   = 1. / resolution;
	int steps = 2;
	float r   = length(wordPos);		//note that lookup is done using texture2DRect (not normalized)
	float r2;
	
	//check jump
	for (float x = -dv.x * steps + TextCoordN.x; x <= dv.x * steps + TextCoordN.x; x+= dv.x)
	{
		for (float y = -dv.y * steps + TextCoordN.y; y <= dv.y * steps + TextCoordN.y; y+= dv.y)
		{
			r2 = length(texture2DRect(worldPosTexture, resolution * vec2(x, y)).xyz);
//			if (r - r2 > maxjump || r2 <= nearDist || r2 >= farDist)
			if (r - r2 > maxjump)
				o = true;
		}
	}
	return o;
}


//-------------------------

void main() {

	vec3 worldPos = texture2DRect(worldPosTexture, gl_MultiTexCoord0.xy).xyz;
		
	//nota - las posiciones de los vertices pueden deformarse en otro fragment shader antes de ser recibidos aquí.
	//dicho shader podría llamarse geometryDeformer y el usuario podría elegir entre varios tipos de deformaciones.
	//geometryDeformer grabaría en un fbo y después usaríamos la textura de ese fbo como imput para las posiciones (ahora usamos worlPosTex)
	
	//spikes or fat mesh deformation
	//if (spikesScale > 0.) {
		if (mod(gl_MultiTexCoord0.x, spikesStep) == 0  &&  mod(gl_MultiTexCoord0.y, spikesStep) == 0 ) {
			worldPos -= (texture2DRect(normalsTexture, gl_MultiTexCoord0.xy).xyz / 1000.) * spikesScale;	//is division by 1000 needed to convert to meters?
		}
	//}
	
	vec4 pos  = gl_ModelViewProjectionMatrix * vec4(worldPos, 1);	//pos = clip space coordinates
	
	bool zero = jumps(gl_MultiTexCoord0.xy);
	pos.z     = zero ? 100. : pos.z;
	existence = zero ? 0. : 1.;
		
	gl_Position    = pos;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	//position = worldPos;	
	position = (clipMatrix * vec4(worldPos, 1)).xyz;	//use position relative to clipMatrix
}