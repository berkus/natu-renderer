#version 120
#extension GL_EXT_draw_instanced: enable  
uniform vec4		wood_amplitudes;
uniform vec4		wood_frequencies;
uniform sampler2D	branch_noise_tex;

attribute vec3		normal;
attribute vec3		tangent;
attribute vec2		texCoords0;
attribute vec2		sliceDescription;
varying vec3		eyeDir;
varying vec3		normalDir;
varying vec3		lightDir_ts;
varying vec3		eyeDir_ts;
varying float		alpha;
varying vec2		sliceDesc;
uniform int			instancing;
attribute mat4		transformMatrix;
attribute vec4		colorVariance;
uniform	vec3		u_colorVariance;
varying vec3		colorVar;

uniform float		time_offset;
varying float		time_offset_v;

varying mat3		TBN_Matrix;
uniform float		time;

uniform mat4		LightMVPCameraVInverseMatrix;
varying	vec4		lightSpacePosition;

void main()
{
	vec4 pos;
	vec3 tangentDir;
	vec3 bitangent;
	bitangent		= cross(normal, tangent);
	//float mv_time;
	
	// = wood_amplitudes.y * ( texture2D(branch_noise_tex, mv1 * mv_time * wood_frequencies.y).rg  * 2.0 - vec2(1.0));
	//vec2 mv1 = normalize(vec2(0.33, 0.47));
	//float amplitude = 3.0;
	if (instancing>0){
		// drawing instances
		colorVar	= colorVariance.rgb;
		time_offset_v = colorVariance.a;
		//mv_time = 0.01 * (time+time_offset_v);
		
		pos			=  ( gl_Vertex * vec4(10.0, 10.0, 10.0, 1.0));	
		//pos.xz		+= texCoords0.y * amplitude * wood_amplitudes.x * (texture2D(branch_noise_tex, 0.01 * pos.xz + mv1 * mv_time * wood_frequencies.x).xy*2.0-vec2(1.0));
		pos			= gl_ModelViewMatrix * (transformMatrix * pos);
		mat3 T		= mat3(transformMatrix);
		tangentDir	= (gl_NormalMatrix * T * tangent	);
		normalDir	= (gl_NormalMatrix * T * normal		);
		bitangent	= (gl_NormalMatrix * T * bitangent	);

	} else {
		// drawing single geometry
		colorVar	= u_colorVariance;
		time_offset_v = time_offset;
		//mv_time		= 0.01 * (time+time_offset);
		pos			=  gl_Vertex * vec4(10.0, 10.0, 10.0, 1.0);
		//pos.xz		+= texCoords0.y * amplitude * wood_amplitudes.x * (texture2D(branch_noise_tex, 0.01 * pos.xz + mv1 * mv_time * wood_frequencies.x).xy*2.0-vec2(1.0));
		pos			= gl_ModelViewMatrix * pos;
		
		tangentDir	= (gl_NormalMatrix * tangent	);
		normalDir	= (gl_NormalMatrix * normal		);		
		bitangent	= (gl_NormalMatrix * bitangent	);
	}

	// calc tangent space...
	// = mat3 (tangent_vs, bitangent, normal_vs);
    TBN_Matrix[0]	=  tangentDir; 
    TBN_Matrix[1]	=  bitangent; 
    TBN_Matrix[2]	=  normalDir; 

	vec3 lpos		= (gl_LightSource[0].position).xyz;
	lightDir_ts		= normalize( lpos ) * TBN_Matrix ;
	eyeDir_ts		= normalize( pos.xyz ) * TBN_Matrix;

	gl_Position		= gl_ProjectionMatrix * pos;
	eyeDir			= pos.xyz;
	sliceDesc		= sliceDescription;
	gl_TexCoord[0]	= vec4(texCoords0, 0.0, 0.0);

	lightSpacePosition = LightMVPCameraVInverseMatrix * pos;

	//alpha = 0.5;
	alpha = clamp(-2.0+5.0*abs(dot(normalize(normalDir.xz), normalize(eyeDir.xz))), 0.0, 1.0);	
	//alpha =clamp(-0.5 + 2.0*abs(dot(normalize(normalDir), normalize(eyeDir))), 0.0, gl_Color.a);
	//alpha = clamp(abs(dot(normalize(normalDir), normalize(eyeDir))), gl_Color.a, 1.0);
	//alpha = gl_Color.a;
	//gl_FrontColor = vec4(normal, alpha);
	gl_FrontColor	= gl_Color;
}

