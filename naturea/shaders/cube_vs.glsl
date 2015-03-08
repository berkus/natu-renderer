/*
* VERTEX SHADER GLSL
*/
attribute vec3 color;

void main(void)
{
	gl_FrontColor = vec4(color, 1.0);
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}