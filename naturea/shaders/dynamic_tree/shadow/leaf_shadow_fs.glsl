#version 120

uniform sampler2D	branch_noise_tex;
uniform sampler2D	frontDecalMap;
uniform sampler2D	seasonMap;
uniform	float		season;

varying float		instanceSpecificNumber;
varying float		leafSpecificNumber;
uniform float		transition_control;
uniform vec2		window_size;

uniform float		dither;
varying vec3		ws_position;

void main()
{	
	
	vec4 decal_color = texture2D(frontDecalMap, gl_TexCoord[0].xy );
	vec2 seasonCoord = vec2(0.5, season + 0.2*leafSpecificNumber - 0.0001*instanceSpecificNumber);
	
	vec4 seasonColor =  texture2D(seasonMap, seasonCoord);
	
	decal_color.a *= seasonColor.a;

	if (decal_color.a < 0.5)
	{
		discard;
	}
	
	vec3 color = vec3(0.0, 0.0, 0.0);
	float treshold = 0.5;
	//gl_FragDepth = gl_FragCoord.z + frontFacing*(depth_tex*2.0 - 1.0)*0.01;
	
	// DITHER //
	float noise = texture2D(branch_noise_tex, gl_FragCoord.xy*0.7*gl_FragCoord.yx*0.033 ).x;
	float transition_control = gl_Color.a; 
	if (transition_control<1.0 && transition_control>0.0 && transition_control<noise){
		discard;
	}
	/*
	if (transition_control<1.0 && transition_control>0.0){
		float c = 2.0*(1.0-transition_control);
		ivec2 screen_pos = ivec2(gl_FragCoord.xy);
		if (mod(screen_pos.x, c)<treshold && mod(screen_pos.y, c)<treshold){
			
			discard;
			//color = vec3(1.0, 0.0, 0.0);
			//gl_FragColor = vec4(color, 1.0);
			//gl_FragDepth = 1.0;
		}
	}
	*/
	gl_FragColor = vec4(color, 1.0-transition_control);

}
