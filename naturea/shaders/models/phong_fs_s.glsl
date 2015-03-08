varying vec3 vNormal, vLightDir, vEyeVec;

void main (void)
{
	
	float h = gl_FrontMaterial.shininess;

	vec3 N = normalize(vNormal);
	vec3 L = normalize(vLightDir);
	vec3 E = normalize(vEyeVec);
	vec3 R = reflect(-L, N);
	float RdotE = max(dot(R, E),0.0);
	float NdotL = max(dot(N, L),0.0);
	float specular = pow( RdotE, h );
	vec4 texColor = gl_FrontLightModelProduct.sceneColor;
	
	vec4 color = texColor * (gl_FrontLightProduct[0].ambient + gl_FrontLightProduct[0].diffuse * NdotL) + gl_FrontLightProduct[0].specular * specular;
	gl_FragData[0] = color;
	gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);	
		
}