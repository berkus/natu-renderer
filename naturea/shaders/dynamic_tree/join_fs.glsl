#version 120

uniform sampler2D colorMap;
uniform sampler2D dataMap;
uniform sampler2D depthMap;
uniform sampler2D normalMap;


//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Fragment Shader
// PREPROCESS
//--------------------------------------------------------------------------------------

void main()
{	
	gl_FragData[0]	= texture2D(colorMap	, gl_TexCoord[0].st);
	gl_FragData[1]	= texture2D(normalMap	, gl_TexCoord[0].st);
	gl_FragData[2]	= texture2D(dataMap		, gl_TexCoord[0].st);
	gl_FragDepth	= texture2D(depthMap	, gl_TexCoord[0].st).r;
}
