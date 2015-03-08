#version 330 compatibility
//==============================================================================
//  Grass FRAGMENT shader   
//
//==============================================================================

uniform sampler2D grass_tex;
varying vec3 normal;
varying vec3 eye;

uniform sampler2D shadowMap;
varying	vec4	lightSpacePosition;
uniform int		shadowMappingEnabled;
uniform int		fastMode;
void main()
{
vec2 st = gl_TexCoord[0].st + vec2(0.0, 0.01);
	vec4 tex_color = texture2D(grass_tex, st);
	if (tex_color.a<0.8){
		discard;
	}
	if (fastMode>0){
		gl_FragData[0] = vec4(1.0, 0.0, 0.0, 1.0);
		return;
	}
	vec3 N = normalize(normal);
	vec3 L = normalize(gl_LightSource[0].position.xyz - eye);

	float NdotL = abs(dot(normalize(N),normalize(L)));

	vec4 Ia = gl_FrontLightProduct[0].ambient;
	vec4 Id = gl_FrontLightProduct[0].diffuse * NdotL;
	
	// shadow
	float shade = 1.0;
	if (shadowMappingEnabled>0){
		// lightSpacePosition = LightProjectionMatrix * LightViewModelMatrix * CameraViewInvereseMatrix * gl_ModelViewMatrix * gl_Vertex
		vec4 l = (lightSpacePosition/lightSpacePosition.w + vec4(1.0,1.0,1.0,1.0))*0.5;
		float depthCamera = l.z;
		float depthLight  = texture(shadowMap, l.xy).r + 0.001;
		if (depthCamera > depthLight){
			shade = 0.5;
		}
	}

	vec4 color = shade * gl_FrontLightModelProduct.sceneColor + (Ia + Id)*tex_color;
	color.a = tex_color.a;
	gl_FragData[0] = color;
	gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);
}