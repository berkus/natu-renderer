#
varying vec3 pos;
varying vec3 lig;
void main(void)
{
	lig = -gl_LightSource[0].position.xyz;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
	pos = (gl_ModelViewMatrix* gl_Vertex).xyz;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}


