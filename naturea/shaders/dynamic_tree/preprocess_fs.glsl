#version 120

uniform sampler2D branchMap;

uniform vec2 window_size;

//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define EPSILON1		30	
#define EPSILON2		30
#define MEPSILON		-10
#define STEP			2.0
//--------------------------------------------------------------------------------------
// Fragment Shader
// PREPROCESS
//--------------------------------------------------------------------------------------

void main()
{	
	vec4 data;
	float minDist = 100000.0;
	float dist;
	vec4 bestData = vec4(0.0);
	vec2 lookUpPos;
	int i = 0;
	int j = 0;
	float dataLength = 0.0;
	for (i=MEPSILON; i<EPSILON1; i++){
		for (j=-EPSILON2; j<EPSILON2; j++){
			// find the closest filled point
			float x = j / window_size.x * STEP;
			float y = i / window_size.y * STEP;
			lookUpPos =  gl_TexCoord[0].xy + (vec2(x,y));
			data = texture2D(branchMap, lookUpPos);

			//bestData = bestData + data;
			dataLength = length(data);
			// if some valid data
			if (dataLength>0.0){
				// calc distance
				dist = length(gl_TexCoord[0].xy-lookUpPos);
				// if better than found so far... 
				if (dist<minDist){
					minDist = dist;
					bestData = data;
				}
			}
			
		}
	}
	
	vec4 color = bestData;
	gl_FragData[0] = color;
}
