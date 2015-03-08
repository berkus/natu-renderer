varying vec3 vNormal, vLightDir, vEyeVec;

void main()
{	
	vNormal = gl_NormalMatrix * gl_Normal;

	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

	vLightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
	vEyeVec = -vVertex;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;		
}