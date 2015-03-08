uniform sampler2D diffuse_texture;

varying vec3 vNormal, vLightDir, vEyeVec;
varying vec4 col;

void main (void)
{
	float h = gl_FrontMaterial.shininess;
	vec3 N = normalize(vNormal);
	vec3 L = normalize(vLightDir);
	vec3 E = normalize(vEyeVec);
	vec3 R = reflect(-L, N);
	float RdotE = max(dot(R, E),0.0);
	float NdotL = max(dot(N, L),0.0);
	float spec = pow( RdotE, h );
	vec4 texColor = texture2D(diffuse_texture, gl_TexCoord[0].st);
	vec4 ambient = gl_LightSource[0].ambient * (gl_FrontMaterial.ambient + 0.4);
	vec4 diffuse = gl_FrontLightProduct[0].diffuse * NdotL;
	vec4 specular = gl_FrontLightProduct[0].specular * spec;
	vec4 color = texColor * (ambient + diffuse) + specular;
	gl_FragData[0] = color*vec4(1.0,  0.0,0.0,1.0);
	gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);	
}