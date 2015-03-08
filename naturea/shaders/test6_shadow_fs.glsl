#version 120
#define SHADOW_TRESHOLD 0.0001

uniform sampler2D	branch_noise_tex;
uniform sampler2D	leaf_noise_tex;
uniform sampler2D	seasonMap;

uniform sampler2D	color_tex_1	;
uniform sampler2D	color_tex_2	;
uniform sampler2D	normal_tex_1;
uniform sampler2D	normal_tex_2;
uniform sampler2D	branch_tex_1;
uniform sampler2D	branch_tex_2;
uniform sampler2D	depth_tex_1;
uniform sampler2D	depth_tex_2;

uniform float		season;
uniform float		time;

uniform vec2		window_size;

uniform vec4		wood_amplitudes;
uniform vec4		wood_frequencies;
uniform float		leaf_amplitude;
uniform float		leaf_frequency;
uniform vec2		movementVectorA;	
uniform vec2		movementVectorB;	
uniform float		near;
uniform	float		far;

varying vec2		sliceDesc;

varying float		time_offset_v;



void	main()
{	
	float dist = gl_FragCoord.z;
	float inv_dist = 1.0/dist;
	vec2 fpos	= gl_TexCoord[0].st;
	vec4 color;
	float sizeFactor = 1.5/max(window_size.x, window_size.y)*0.5;
	float t	= 5.0*(time+time_offset_v)*leaf_frequency*sizeFactor;
	vec2 texCoordA	= fpos+t*movementVectorA;
	vec2 texCoordB	= fpos+t*movementVectorB;
	texCoordA = (texture2D(leaf_noise_tex, texCoordA).st*2.0 - vec2(1.0));
	texCoordB = (texture2D(leaf_noise_tex, texCoordB).st*2.0 - vec2(1.0));
	vec2 noiseOffset = (texCoordA+texCoordB)*sizeFactor*leaf_amplitude*0.5;
	vec2 newPos = fpos;
	vec2 b0 = vec2(0.5,0.0);
	vec2 b1;
	if (sliceDesc.y>0.0){
		b1 = texture2D(branch_tex_2, fpos).zw;
	} else {
		b1 = texture2D(branch_tex_1, fpos).zw;
	}
	float dist0 = min (1.0, 2.0 * length(fpos - b0)) ; // / branchProjectedLength
	float dist1 = min (1.0, 5.0 * length(fpos - b1)); // / branchProjectedLength

	float angle;
	float cosA;
	float sinA;
	vec2  difVec;
	mat2 R;
	vec2 rotatedDifVec;
	float ti = (time+time_offset_v)*sizeFactor*10.0;
	vec2 si = sizeFactor* 100.0 * wood_amplitudes.xy;
	float d = length(b1-vec2(0.5));

	if ((d>0.01)){
		angle = dist1*(texture2D(branch_noise_tex, (ti * b1 * wood_frequencies.y)).s*2.0 - 1.0)  * si.y * 2.0;
		cosA = cos (angle); 
		sinA = sin (angle);
		difVec = (newPos - b1);
		R = mat2(	 cosA	, sinA,
 					-sinA	, cosA );
		rotatedDifVec = R*difVec;
		newPos = b1 + rotatedDifVec;
	}
	
	angle = dist0 * (texture2D(branch_noise_tex, (ti * b0 * wood_frequencies.x)).s*2.0-1.0) * si.x * 0.5;
	cosA = cos (angle); 
	sinA = sin (angle);
	difVec = (newPos - b0);
	R = mat2(	 cosA	, sinA,
 				-sinA	, cosA );
	rotatedDifVec = R*difVec;
	newPos = b0 + rotatedDifVec;
	vec2 texCoord = newPos + noiseOffset ;
	float fragmentNormalLeaf  ;
	float fragmentNormalBranch;
	if (sliceDesc.y>0.0){
		// 90 degrees rotated billboard
		fragmentNormalLeaf   = texture2D(normal_tex_2, texCoord).w;
		fragmentNormalBranch = texture2D(normal_tex_2, newPos).w;		
	} else {
		fragmentNormalLeaf   = texture2D(normal_tex_1, texCoord).w;
		fragmentNormalBranch = texture2D(normal_tex_1, newPos).w;
	}
	float branchFlag = fragmentNormalBranch + fragmentNormalLeaf;
	vec2 lookUpPos;
	float leaf = 0.0;
	if (branchFlag<0.1){
		// trunk / branch 
		lookUpPos = newPos;
	} else {
		// foliage
		lookUpPos = texCoord;
		leaf = (1.0/0.9)*(fragmentNormalLeaf-0.1);
	}
	float frontFacing = -1.0;
	if (gl_FrontFacing){
		frontFacing = 1.0;
	}

	vec4 decal_color;
	float depth_tex;
	if (sliceDesc.y>0.0){
		decal_color = texture2D(color_tex_2, lookUpPos);
		depth_tex = texture2D(depth_tex_2, lookUpPos).x;
	} else {
		decal_color = texture2D(color_tex_1, lookUpPos);
		depth_tex = texture2D(depth_tex_1, lookUpPos).x;
	}
	// escape when transparent...
	if (decal_color.a<0.5){discard;}
	//vec3 variation = colorVar;
	if (branchFlag>=0.1){
		// leaf
		vec2 seasonCoord = vec2(0.5, season + 0.2*leaf - 0.0001*time_offset_v);
		
		vec4 seasonColor =  texture2D(seasonMap, seasonCoord);
		// discard leaf
		if (seasonColor.a<0.5){
			discard;
		}
	} 
	// compose color (phong)
	gl_FragData[0] = vec4(1.0, 0.0, 0.0, 1.0);
	gl_FragDepth = gl_FragCoord.z + frontFacing*(depth_tex*2.0 - 1.0)*0.02;
	return;
}
