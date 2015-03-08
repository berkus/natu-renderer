
varying vec3 vNormal, vLightDir, vEyeVec, vBinormal, vTangent;

varying vec3 vLightDirT, vEyeVecT, vNormalT, vBinormalT, vTangentT;

attribute vec3 a_binormal;
attribute vec3 a_facenormal;

void main() 
{ 
	vec3 binormal	= a_binormal;
	vec3 facenormal	= a_facenormal;
	vec3 normal		= gl_Normal;
	
	vec3 tangent	= cross(-normal, binormal);
	vBinormal		= binormal;
	vTangent		= tangent;
	// matice prechodu z eye space -> tangent space
	mat3 TBN_Matrix;
    TBN_Matrix[0] =  gl_NormalMatrix * tangent; 
    TBN_Matrix[1] =  gl_NormalMatrix * binormal; 
    TBN_Matrix[2] =  gl_NormalMatrix * normal; 

	vec3 vertexE = vec3(gl_ModelViewMatrix * gl_Vertex);
	// smer k pozorovateli v tangent space
	vEyeVecT = vertexE * TBN_Matrix;    // normalizovat az ve FS 
    // normala v tangent space
	vNormalT	= (gl_NormalMatrix	* normal	)* TBN_Matrix;
	vBinormalT	= (gl_NormalMatrix	* binormal	)* TBN_Matrix;
	vTangentT	= (gl_NormalMatrix	* tangent	)* TBN_Matrix;



	// smer ke svetlu v tangent space
	// positional light
	vec3 lightE = vec3(gl_ModelViewMatrix * gl_LightSource[0].position);
    if(gl_LightSource[0].position.w == 1.0) 
    {
		vLightDirT = (lightE - vertexE) * TBN_Matrix;
	}
	else // directional light
	{
		vLightDirT = lightE * TBN_Matrix;
	}


	vNormal = gl_NormalMatrix * normal;

	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

	vLightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
	vEyeVec = -vVertex;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;	

	gl_TexCoord[0]	= gl_MultiTexCoord0;
	
	gl_FrontColor	= gl_Color;
}