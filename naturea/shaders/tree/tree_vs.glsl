#
//==============================================================================
//  Tree1 VERTEX shader   
//
//==============================================================================
	
varying vec3	eye;
varying vec3	normal;

// shadow mapping
uniform mat4	LightMVPCameraVInverseMatrix;
varying	vec4	lightSpacePosition;

void main()
{
	normal = gl_NormalMatrix * gl_Normal;	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
	vec4 position = gl_ModelViewMatrix * gl_Vertex;
	eye = vec3(position);

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;//gl_ModelViewMatrix * gl_Vertex;
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	lightSpacePosition = LightMVPCameraVInverseMatrix*(position);
	
	//gl_FrontColor = vec4(d*0.5+0.5, 0.0, 1.0);
	
}