#version 120
#extension GL_EXT_draw_instanced: enable  

attribute vec3		normal;
attribute vec3		tangent;
attribute vec2		texCoords0;
attribute vec2		sliceDescription;

varying vec2		sliceDesc;
uniform int			instancing;
attribute mat4		transformMatrix;
attribute vec4		colorVariance;
uniform float		time_offset;
varying float		time_offset_v;

uniform float		time;
void main()
{
	vec4 pos;
	if (instancing>0){
		// drawing instances
		time_offset_v	= colorVariance.a;
		pos			=  ( gl_Vertex * vec4(10.0, 10.0, 10.0, 1.0));	
		pos			= gl_ModelViewMatrix * (transformMatrix * pos);

	} else {
		// drawing single geometry
		time_offset_v = time_offset;
		pos			= gl_Vertex * vec4(10.0, 10.0, 10.0, 1.0);
		pos			= gl_ModelViewMatrix * pos;
	}

	// calc tangent space...
	// = mat3 (tangent_vs, bitangent, normal_vs);
   
	gl_Position		= gl_ProjectionMatrix * pos;
	sliceDesc		= sliceDescription;
	gl_TexCoord[0]	= vec4(texCoords0, 0.0, 0.0);
	gl_FrontColor	= gl_Color;
}

