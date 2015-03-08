#extension GL_ARB_geometry_shader4 : enable

varying in vec3 normal_vs	[3];
varying in vec3 tangent_vs	[3];

float dist = 0.5;
void main (){
	int i;
	
	for(i=0; i<gl_VerticesIn; i++)
	{ 
		gl_FrontColor = vec4(0.0, 0.0, 1.0, 1.0);
		gl_Position = gl_ProjectionMatrix * ( gl_PositionIn[i] + vec4(tangent_vs[i],1.0)* dist) ;
		EmitVertex();
		gl_FrontColor = vec4(1.0, 1.0, 1.0, 1.0);
		gl_Position = gl_ProjectionMatrix * gl_PositionIn[i];
		EmitVertex();
		gl_FrontColor = vec4(0.5, 0.0, 0.0, 1.0);
		gl_Position =  gl_ProjectionMatrix * ( gl_PositionIn[i] + vec4(normal_vs[i],1.0)* dist) ;
		EmitVertex();
		EndPrimitive();
	}
	/* render leaf */
	gl_FrontColor = vec4(0.0, 1.0, 0.0, 1.0);
	for (i=0; i<gl_VerticesIn; i++){
		gl_Position = gl_ProjectionMatrix * ( gl_PositionIn[i] );
		EmitVertex();
	}
	EndPrimitive();
   
}