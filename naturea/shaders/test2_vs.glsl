#version 120

attribute vec3			normal;
attribute vec3			tangent;
attribute vec2			texCoords0;

varying vec3			eyeDir;
varying vec3			normalDir;
varying float			alpha;
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	eyeDir = (gl_ModelViewMatrix * gl_Vertex).xyz;
	normalDir = (gl_NormalMatrix * normal);
	
	gl_TexCoord[0] = vec4(texCoords0, 0.0, 0.0);
	//alpha =clamp(-0.5 + 2.0*abs(dot(normalize(normalDir), normalize(eyeDir))), 0.0, gl_Color.a);
	//alpha = clamp(abs(dot(normalize(normalDir), normalize(eyeDir))), gl_Color.a, 1.0);
	alpha = gl_Color.a;
	gl_FrontColor = vec4(normal, alpha);
}

