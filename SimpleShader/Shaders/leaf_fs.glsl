#

uniform sampler2D color_texture;

void main()
{	
	vec4 color = texture2D(color_texture, gl_TexCoord[0].xy);
	
	if (color.a<=0.0){
		discard;
	}
	gl_FragColor = color;
	//gl_FragColor = vec4(gl_Color.xyz, 1.0);
	//gl_FragColor = vec4(0.0, 0.5, 0.0, 1.0);
	//gl_FragColor = vec4(gl_TexCoord[0].xy, 0.0, 1.0);

}