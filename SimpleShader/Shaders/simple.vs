#
uniform sampler2D my_texture;
uniform vec2	  my_texture_steps;

void main()
{
	vec4 vertex = gl_Vertex;
	vec4 values = texture2D( my_texture, gl_MultiTexCoord0.xy );
	vertex.x = vertex.x + values.x;
	//vertex = vertex + values;
	gl_FrontColor = gl_Color+values; 
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
