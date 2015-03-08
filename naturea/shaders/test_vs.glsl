#version 120

attribute vec3			normal;
attribute vec3			tangent;
vec3					binormal;
attribute vec2			texCoords0;

varying vec3			normal_vs_v;
varying vec3			tangent_vs_v;
varying vec3			ts_viewDir_v;
varying vec3			ts_lightDir_v;

vec3					normal_vs = normal;
vec3					tangent_vs = tangent;
vec4					color;	
varying vec3			o_normal;


void main()
{
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	

	 vec3 vertex = gl_Vertex.xyz;
    o_normal = normal_vs;
	vec3 bitangent = cross (normal_vs, tangent_vs);

	// TBN matrix & coord system transfer to Tangent space...

	mat3 TBN_Matrix;// = mat3 (tangent_vs, bitangent, normal_vs);
    TBN_Matrix[0] =  gl_NormalMatrix * tangent_vs; 
    TBN_Matrix[1] =  gl_NormalMatrix * bitangent; 
    TBN_Matrix[2] =  gl_NormalMatrix * normal_vs; 

	// Transform view vector into tangent space.
	vec3 view_vector = vec3(gl_ModelViewMatrix * gl_Vertex);
	ts_viewDir_v = normalize( view_vector * TBN_Matrix) ;
	
	// Calculate vector to light in tangent space. (directional light)
	ts_lightDir_v = normalize(gl_LightSource[0].position.xyz * TBN_Matrix );


	normal_vs = gl_NormalMatrix * normal_vs;
	//vPos = gl_ModelViewMatrix * vec4(vertex,1.0);
	
	gl_TexCoord[0] = vec4(texCoords0, 0.0, 0.0);
	//gl_FrontColor = color;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex,1.0);

	tangent_vs_v = tangent_vs;
	normal_vs_v  = normal_vs; 
	
}

