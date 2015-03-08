#
//==============================================================================
//  God Rays VERTEX shader   
//  inspired by: http://fabiensanglard.net/lightScattering/
//==============================================================================

void main(void)
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}


