#
//==============================================================================
//  Water VERTEX shader   
//  inspired by: http://forum.bonzaisoftware.com/viewthread.php?tid=10
//				 http://www.bonzaisoftware.com/water_tut.html#refl
//==============================================================================

varying vec4 projectedVertex;


void main(void)
{
	gl_TexCoord[0] =  gl_MultiTexCoord0;
	
	projectedVertex = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position = projectedVertex;
}


