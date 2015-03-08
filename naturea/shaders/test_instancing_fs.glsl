#version 120
void main()
{	
	vec4 color		= vec4(1.0,0.0,0.0,0.5);
	gl_FragData[0]	= color;
	gl_FragData[1]	= color * vec4(0.5, 0.5, 0.5, 1.0);

}
