#
uniform vec4 skycolor;
uniform sampler2D skybox_tex;
varying vec3 pos;
varying vec3 lig;
void main(void)
{
	vec4 origColor = texture2D(skybox_tex, gl_TexCoord[0].st);
	
	vec3 P = normalize(pos);
	float PdotL =(0.5 * (dot(P,lig))+0.5);

	//gl_FragData[0] = vec4(1.0,1.0,1.0,1.0);
	
	gl_FragData[0] = origColor;
	//skycolor.a = 0.5;
	gl_FragData[1] = origColor * clamp(PdotL, 0.0, 0.5);
}

