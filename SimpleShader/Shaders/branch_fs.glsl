#


varying vec4 normal_vs;
varying vec4 tangent_vs;
varying int level;

uniform sampler2D color_texture;

void main()
{	
	gl_FragColor = texture2D(color_texture, gl_TexCoord[0].xy);


	//gl_FragColor = gl_Color;
}