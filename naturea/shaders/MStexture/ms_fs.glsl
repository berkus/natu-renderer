/*fragment shader*/
#version 150
uniform sampler2DMS colorMap;
uniform float samples;
uniform vec2 size;
void main(){
	vec4 color = vec4(0.0,0.0,0.0,0.0);
	for (int i=0; i<1; i++){
		color+= texelFetch(colorMap,  ivec2(gl_TexCoord[0].xy*size),  i);
		//color+= texture2D(colorMap,  gl_TexCoord[0].xy);
	}
	color = color / 1.0;
	color.a = 1.0;
	gl_FragColor = color;
}