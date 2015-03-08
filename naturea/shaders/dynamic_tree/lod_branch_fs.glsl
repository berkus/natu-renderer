#version 120

varying vec3			normal_v;
varying vec3			normal_vs;
varying vec3			tangent_vs;
varying vec4			vPos;

varying vec2			b0_origin;
varying vec3			b1_origin;
varying vec2			b2_origin;

varying float			branchID;
uniform sampler2D		color_texture;
uniform vec2			window_size;

//uniform vec3			cam_dir;
//uniform vec3			cam_right;
//vec3					cam_up = cross(cam_dir, cam_right);
varying vec2			mv_v;

void main()
{	

	vec4 texColor = texture2D(color_texture, gl_TexCoord[0].xy);

	gl_FragData[0] = texColor;

	vec3 nor = normalize(normal_vs);
	gl_FragData[1] = vec4(nor , 0.0);
	//gl_FragData[1] = vec4(nor*0.5 + vec3(0.5) , 1.0);
	//gl_FragData[1] = vec4(normal*0.5 + vec3(0.5) , 1.0);
	//gl_FragData[2] = vec4(b0_origin*0.5+vec2(0.5),b1_origin*0.5+vec2(0.5));
	
	gl_FragData[2] = vec4(-branchID);
	//gl_FragData[2] = vec4(normalize(mv_v),b1_origin*0.5+vec2(0.5));
}