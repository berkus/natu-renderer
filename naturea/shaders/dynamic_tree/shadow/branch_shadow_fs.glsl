#version 120
uniform float		transition_control;
uniform vec2		window_size;
void main()
{	
	/*
	ivec2 screen_position = ivec2(window_size.xy * (gl_FragCoord.xy*0.5 + vec2(0.5)));
	if ( 0 < mod(screen_position.x+screen_position.y,2.0)){
		//	odd
		discard;
	} else {
		// even
	}*/
	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	
	//gl_FragDepth = gl_FragCoord.z + 0.001*transition_control*transition_control;
	
}