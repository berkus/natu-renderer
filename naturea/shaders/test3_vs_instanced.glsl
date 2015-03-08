#version 120

attribute vec3			normal;
attribute vec3			tangent;
attribute vec2			texCoords0;
attribute vec2			sliceDescription;
varying vec3			eyeDir;
varying vec3			normalDir;
varying float			alpha;
varying vec2			sliceDesc;
attribute mat4			transformMatrix;
varying vec3			lightDir_ts;

#define LOG2 1.442695
void main()
{
	vec4 pos = gl_ModelViewMatrix * transformMatrix * ( gl_Vertex * vec4(10.0, 10.0, 10.0, 1.0));
	gl_Position = gl_ProjectionMatrix * pos;
	
	eyeDir = pos.xyz;
	
	mat3 T = mat3(transformMatrix);

	normalDir = (gl_NormalMatrix * T * normal);
	
	// calc tangent space...
	vec3 bitangent = cross (normal, tangent);
	mat3 TBN_Matrix;// = mat3 (tangent_vs, bitangent, normal_vs);
    TBN_Matrix[0] =  gl_NormalMatrix * tangent; 
    TBN_Matrix[1] =  gl_NormalMatrix * bitangent; 
    TBN_Matrix[2] =  gl_NormalMatrix * normal; 

	lightDir_ts = normalize( (gl_LightSource[0].position).xyz * TBN_Matrix );

	//sliceDesc = sliceDescription;
	//gl_TexCoord[0] = vec4(texCoords0, 0.0, 0.0);

	//alpha =clamp(-0.5 + 2.0*abs(dot(normalize(normalDir), normalize(eyeDir))), 0.0, gl_Color.a);
	//alpha = clamp(abs(dot(normalize(normalDir), normalize(eyeDir))), gl_Color.a, 1.0);
	//alpha = gl_Color.a;
	//gl_FrontColor = vec4(normal, alpha);
	
	gl_FrontColor = gl_Color;
}

