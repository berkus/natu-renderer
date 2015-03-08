uniform sampler2D diffuse_texture;
uniform sampler2D bump_texture; // but height data

varying vec3 vNormal, vLightDir, vEyeVec, vBinormal, vTangent;
varying vec3 vLightDirT, vEyeVecT, vNormalT, vBinormalT, vTangentT;

uniform int enabled;
uniform vec2 scaleBias;

varying vec4 col;

void main (void)
{
	// pozorovatel
	vec3 eye = normalize(vEyeVecT);
	
	// svetlo
	vec3 L = normalize(vLightDir);
	
    vec2 srcUV = gl_TexCoord[0].xy;    

    float height = texture2D(bump_texture, srcUV).r-0.5;

	//float hsb = height*0.01;
    float hsb = height * scaleBias.y + scaleBias.x;
    
    vec2 texUV = srcUV + (hsb * eye.xy);   // offset limiting     
    //vec2 texUV = srcUV + (hsb * eye.xy/eye.z);    
	
	vec4 color;
	if (enabled>0){
		color = texture2D(diffuse_texture, texUV);
	} else {
		color = texture2D(diffuse_texture, srcUV);
	}
	
    // lighting
    vec4 ambient = gl_LightSource[0].ambient *(gl_FrontMaterial.ambient + 0.4);
    vec4 diffuse = vec4(0.0); // difùzní
    vec4 specular = vec4(0.0); // zrcadlová
    
	float STEP = 0.001;
	vec2 cmox = vec2(srcUV.x - STEP, srcUV.y);
	vec2 cpox = vec2(srcUV.x + STEP, srcUV.y);
	vec2 cmoy = vec2(srcUV.x, srcUV.y - STEP);
	vec2 cpoy = vec2(srcUV.x, srcUV.y + STEP);

	float dx = texture2D(bump_texture, cmox).r - texture2D(bump_texture, cpox).r ;
	float dy = texture2D(bump_texture, cmoy).r - texture2D(bump_texture, cpoy).r ;
	// normala
	vec3 N = vNormal + vec3(dx, dy, 0.0);
	N = normalize(N);
	// Lambert term
    float NdotL = max(dot(N, L), 0.0);
	diffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * NdotL; 
	vec3 R = reflect(-L, N); 
	vec3 E = normalize(vEyeVec);
	float RdotE = max(0.0, dot(R, E));
	specular = gl_LightSource[0].specular * gl_FrontMaterial.specular * pow(RdotE, gl_FrontMaterial.shininess);
    
    //gl_FragColor = ((ambient + diffuse ) * rgb *1.5  +  specular * 2.0);  //2x svetlejsi
	//gl_FragData[0] = color;
	gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);	
	gl_FragData[0] = ((ambient + diffuse ) * color  +  specular);

	/*
	float h			= gl_FrontMaterial.shininess;
	vec3 N			= normalize(vNormal);
	vec3 L			= normalize(vLightDir);
	vec3 E			= normalize(vEyeVec);
	vec3 R			= reflect(-L, N);
	float RdotE		= max(dot(R, E),0.0);
	float NdotL		= max(dot(N, L),0.0);
	float spec		= pow( RdotE, h );
	vec4 height		= texture2D(bump_texture, gl_TexCoord[0].st);
	vec4 texColor	= texture2D(diffuse_texture, gl_TexCoord[0].st);
	vec4 ambient	= gl_LightSource[0].ambient * (gl_FrontMaterial.ambient + 0.4);
	vec4 diffuse	= gl_FrontLightProduct[0].diffuse * NdotL;
	vec4 specular	= gl_FrontLightProduct[0].specular * spec;
	vec4 color		= texColor * height * (ambient + diffuse) + specular;
	gl_FragData[0]	= color;
	gl_FragData[1]	= vec4(0.0, 0.0, 0.0, 1.0);	
	*/
}