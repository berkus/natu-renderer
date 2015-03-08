#version 150 compatibility
#extension GL_EXT_gpu_shader4 : enable
//==============================================================================
//  God Rays FRAGMENT shader   
//  inspired by: http://fabiensanglard.net/lightScattering/
//==============================================================================
uniform sampler2DMS rtex;
uniform sampler2DMS otex;
//uniform sampler2D btex;

uniform vec3		tintColor;
uniform float		tintFactor;

uniform	float		expo;
uniform	float		decay;
uniform	float		density;
uniform	float		weight;
uniform	float		illuminationDecay;

uniform float		bloomDivide;

const int			NUM_SAMPLES = 32;
const int			BLOOM_SAMPLES = 4;
float				bloomDistance = 0.03/BLOOM_SAMPLES;
uniform vec2		lightPositionOnScreen;
uniform float		lightDirDOTviewDir;
uniform int			sampleCount;
ivec2				texCoord;

vec4 texture2DMSrtex(ivec2 coord, int count){
	vec4  color = vec4(0.0);
	for (int j=0; j<count; j++){
		color += texelFetch(rtex, coord, j);
	}
	color /= float(count);
	return color;
}

vec4 texture2DMSotex(ivec2 coord, int count){
	vec4  color = vec4(0.0);
	for (int j=0; j<count; j++){
		color += texelFetch(otex, coord, j);
	}
	color /= float(count);
	return color;
}


void main(void)
{
	
	ivec2 texSize = textureSize(otex).xy;
	texCoord = ivec2(gl_TexCoord[0].st * texSize);
	vec4  origColor = vec4(0.0);
	origColor += texelFetch(otex, texCoord, 0);
	
	/*
	// makes troubles on ATI card... loops with dynamic loop-count
	for (int j=0; j<sampleCount; j++){
		origColor += texelFetch(otex, texCoord, j);
	}
	
	origColor /= float(sampleCount);
	*/
	//origColor = texture2DMSotex(texCoord, sampleCount);
	origColor.a = 1.0;
	origColor.rgb = pow(origColor.rgb,vec3(1.0/1.2));

	/*
	vec4 bloom = vec4(0.0);
	float xd,yd,l;
	bloomDistance*= float (texSize.x);
	float divideFactor = bloomDivide * BLOOM_SAMPLES*BLOOM_SAMPLES;
	//if (length(raysColor.xyz)<0.4){
		for (int x=-BLOOM_SAMPLES; x<BLOOM_SAMPLES; x++){
			for (int y=-BLOOM_SAMPLES; y<BLOOM_SAMPLES; y++){
				xd = float(x)/float(BLOOM_SAMPLES);
				yd = float(y)/float(BLOOM_SAMPLES);
				l = sqrt(xd*xd+yd*yd);
				//vec4 c = texture2D(rtex, gl_TexCoord[0].st + vec2(xd,yd)*l*bloomDistance);
				vec4 c = texelFetch(rtex, ivec2(texCoord + vec2(xd,yd)*l*bloomDistance), 0);
				if (length(c.rgb)>0.1){
					bloom += c;
					divideFactor+= l;
				}
			}
		}
		origColor += (bloom/divideFactor);
		
	//}
	*/
	
	//====================================================================
	// Sun position on screen - check algorithm
	//--------------------------------------------------------------------
	//vec2 l = gl_TexCoord[0].st - lightPositionOnScreen;
	//if (length(l)<0.03){
	//	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//} else {
	//	gl_FragColor = origColor;
	//}
	//return;
	//--------------------------------------------------------------------
	vec3  raysColor = vec3(0.0);
	vec3  godColor = vec3(0.0);
	float illumDec =illuminationDecay;
	if (lightDirDOTviewDir>0.0){
		float exposure	= expo/NUM_SAMPLES;
		//float decay		= 1.0;
		//float density	= 0.8;
		//float weight	= 6.0;
		//float illuminationDecay = 1.1;

		vec2 deltaTextCoord = vec2( gl_TexCoord[0].st - lightPositionOnScreen);
		vec2 textCoo = gl_TexCoord[0].st;
		deltaTextCoord *= 1.0 / float(NUM_SAMPLES) * density;
		for(int i=0; i < NUM_SAMPLES ; i++)
		{
			textCoo -= deltaTextCoord;
			//vec3 tsample = texelFetch(rtex, ivec2(textCoo*texSize) , 0).rgb;
			vec3 tsample = texelFetch(rtex, ivec2( textCoo*texSize ), 0).rgb;
			//vec4 tsample = texture2D(rtex, textCoo );
			//if (length(tsample.rgb)<0.5){
				tsample *= illumDec * weight;
				raysColor += tsample;
				illumDec *= decay;
			//}
		}
		raysColor *= exposure * lightDirDOTviewDir;
		godColor += raysColor.rgb;
	}
	gl_FragColor = vec4(godColor.rgb + origColor.rgb, 1.0);
	//gl_FragColor = vec4(origColor.rgb, 1.0);
	
}

