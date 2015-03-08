#version 120

attribute mat4			transformMatrix;

void main()
{	
	gl_Position = gl_ModelViewProjectionMatrix * transformMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
}

