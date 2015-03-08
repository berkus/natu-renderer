#version 120
#define SHADOW_TRESHOLD 0.0001

varying vec3			normal_vs;
varying vec3			tangent_vs;
varying float			level;
varying vec4			vPos;
varying vec3			lightDir;

varying vec2			b0_origin;
varying vec2			b1_origin;
varying vec2			b2_origin;

uniform sampler2D		color_texture;
uniform vec2			window_size;

uniform int		shadowMappingEnabled;
uniform sampler2DShadow shadowMap;
varying vec4	lightSpacePosition;
vec4 lpos;

const float infinity = 999999999;
/*
float getDepth(vec2 coords){
	if (clamp(coords.xy, 0.0, 1.0)!= coords.xy){
		return infinity; // infinity
	}

	float depth =  texture2D(shadowMap, coords.xy).x;
	if (depth>=1.0){
		return infinity;
	}
	return depth;
}*/

float getShadow(vec2 position, vec2 offset, float depth){
	return shadow2D(shadowMap, vec3(position+offset*0.001,depth-SHADOW_TRESHOLD)).r; 
}

float getShadowIntensity(vec4 sm_pos){
	float res = getShadow(sm_pos.xy, vec2(0.0, 0.0), sm_pos.z) * 2.0;
	res += getShadow(sm_pos.xy, vec2(1.0, 0.0), sm_pos.z);
	res += getShadow(sm_pos.xy, vec2(-1.0, 0.0), sm_pos.z);
	res += getShadow(sm_pos.xy, vec2(0.0, 1.0), sm_pos.z);
	res += getShadow(sm_pos.xy, vec2(0.0, -1.0), sm_pos.z);
	return res/6.0;
}

void main()
{	

//vec4 color = texture2D(color_texture, gl_TexCoord[0].xy);
	float h = gl_FrontMaterial.shininess;
	vec3 lightDir = -gl_LightSource[0].position.xyz;

	vec3 N = normalize(normal_vs);
	vec3 L = normalize(lightDir);
	vec3 E = normalize(-vPos.xyz);
	vec3 R = reflect(-L, N);
	float RdotE = max(dot(R, E),0.0);
	float NdotL = max(dot(N, L),0.0);
	//float RdotE = abs(dot(R, E));
	//float NdotL = abs(dot(N, L));
	float spec = pow( RdotE, h );
	vec4 texColor = texture2D(color_texture, gl_TexCoord[0].xy);
	
	//vec3 color = (texColor * diffuse).xyz;

	// SHADOW MAPPING //
	float shadow_intensity = 1.0;
	if (shadowMappingEnabled>0){
		shadow_intensity = getShadowIntensity(lightSpacePosition);
	}
	// END SHADOW MAPPING //
	vec4 ambient = gl_LightSource[0].ambient;
	vec4 diffuse = gl_FrontLightProduct[0].diffuse * NdotL * shadow_intensity;
	vec4 specular = gl_FrontLightProduct[0].specular * spec * shadow_intensity;
	vec3 color = ((texColor) * (ambient + diffuse) + specular).xyz;

	//gl_FragData[0] = vec4(vec3(shade), 1.0);
	gl_FragData[0] = vec4(color, gl_Color.a);
	//gl_FragData[1] = color * vec4(0.5, 0.5, 0.5, 1.0);
	gl_FragData[1] =vec4(0.0, 0.0, 0.0, 1.0);
	//gl_FragColor = vec4(1.0/level * texture2D(color_texture, gl_TexCoord[0].xy).xyz, 1.0);
	//gl_FragData[2] = vec4(normal_vs, 1.0);


	// calc frag distance from its branch origin... 
/*	vec2 fpos = gl_FragCoord.xy;
	fpos = fpos/window_size;
	vec2 b0 = b0_origin * 0.5 + 0.5;
	float dist = length(fpos - b0);
	*/
	//gl_FragData[3] = vec4(b0_origin*0.5+vec2(0.5),b1_origin*0.5+vec2(0.5));
	//gl_FragColor = gl_Color;
}