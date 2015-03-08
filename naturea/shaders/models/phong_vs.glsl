
varying vec3 vNormal, vLightDir, vEyeVec;
varying vec4 col;

attribute vec3 a_binormal;
attribute vec3 a_facenormal;

void main() 
{ 
	vec3 binormal	= a_binormal;
	vec3 facenormal = a_facenormal;
	//col = vec4(binormal,1.0);
    //vNormal = gl_NormalMatrix * gl_Normal;
	vNormal = gl_NormalMatrix * facenormal;

	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

	vLightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
	vEyeVec = -vVertex;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;	

	gl_TexCoord[0]	= gl_MultiTexCoord0;
	
	gl_FrontColor	= vec4(binormal, 1.0);//gl_Color;
}