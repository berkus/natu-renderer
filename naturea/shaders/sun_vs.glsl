/*
* VERTEX SHADER GLSL
*/

void main(void)
{
	gl_FrontColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}