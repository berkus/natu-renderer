#version 120

/*************************************************
* HIERARCHICAL VERTEX DISPLACEMENT
**************************************************/
#define	texCols  18.0
#define WHITE	vec4(1.0, 1.0, 1.0, 1.0)
#define RED		vec4(1.0, 0.0, 0.0, 1.0)
#define GREEN	vec4(0.0, 1.0, 0.0, 1.0)
#define BLUE	vec4(0.0, 0.0, 1.0, 1.0)
#define BLACK	vec4(0.0, 0.0, 0.0, 1.0)
#define ONE2    vec2(1.0,1.0)
#define ONE3    vec3(1.0,1.0,1.0)
#define ONE4    vec4(1.0,1.0,1.0,1.0)
#define EPSILON 0.0001
uniform float			branch_count;
uniform float			time;

uniform float			time_offset_leaves;

uniform sampler2D		data_tex;
uniform sampler2D		branch_noise_tex;
uniform sampler2D		leaf_noise_tex;

uniform vec3			wind_direction;
uniform float			wind_strength;
uniform vec4			wood_amplitudes;
uniform vec4			wood_frequencies;
uniform float			leaf_amplitude;
uniform float			leaf_frequency;
uniform vec2			window_size;

attribute vec3			normal;
attribute vec3			tangent;
vec3					binormal;
attribute vec4			x_vals;
attribute float			branch_index;
attribute vec2			texCoords0;

varying vec3			normal_vs;
varying vec3			tangent_vs;
varying vec3			ts_viewDir_v;
varying vec3			ts_lightDir_v;
varying float			level;

varying vec2			b0_origin;
varying vec2			b1_origin;
varying vec2			b2_origin;
varying vec3			b_lengths;

varying vec4			vPos;
						
vec4					color;	
vec3					oVec=vec3(1.0, 1.0, 1.0);

vec4 OS2ND(in vec4 position){
	vec4 clipSpacePosition = gl_ModelViewProjectionMatrix * position;
	//float wI = 1.0/clipSpacePosition.w;
	float wI = 1.0;
	return (clipSpacePosition*wI);
}

void animateBranchVertex(inout vec3 position)
{
//===========================================================
// Displacement method inspired by Ralf Habels article: 
//  Physically Guided Animation of Trees
//
//===========================================================
	vec3 br, bs, bt;
    //vec3 animated_vertex = position;
	
    float ttime = time;
    float mv_time = time* 0.01;
   
    //function for alpha = 0.1
	//vec4 xvals_f = 0.3326*pow4(xvals,2.0) + 0.398924*pow4(xvals,4.0);
	//vec4 xvals_deriv = 0.665201*xvals + 1.5957*pow4(xvals,3.0);
	
	//function for alpha = 0.2
	vec4 xvals_f = 0.374570*x_vals*x_vals + 0.129428*x_vals*x_vals*x_vals*x_vals;
    vec4 xvals_deriv = 0.749141*x_vals + 0.517713*x_vals*x_vals*x_vals;


	
	
	// get coord systems from branch data texture
	vec4 sv0_l	= texture2D(data_tex, vec2(2.5/texCols, branch_index/branch_count));
    vec4 sv1_l	= texture2D(data_tex, vec2(3.5/texCols, branch_index/branch_count));
    vec4 sv2_l	= texture2D(data_tex, vec2(4.5/texCols, branch_index/branch_count));
    vec4 sv3_l	= texture2D(data_tex, vec2(5.5/texCols, branch_index/branch_count));
    vec3 rv0	= texture2D(data_tex, vec2(6.5/texCols, branch_index/branch_count)).xyz;
    vec3 rv1	= texture2D(data_tex, vec2(7.5/texCols, branch_index/branch_count)).xyz;
    vec3 rv2	= texture2D(data_tex, vec2(8.5/texCols, branch_index/branch_count)).xyz;
    vec3 rv3	= texture2D(data_tex, vec2(9.5/texCols, branch_index/branch_count)).xyz;
    // motion vectors
	vec4 mv01 = texture2D(data_tex, vec2(0.5/texCols, branch_index/branch_count));
    vec4 mv23 = texture2D(data_tex, vec2(1.5/texCols, branch_index/branch_count));
    vec2 mv0 = mv01.xy;
    vec2 mv1 = mv01.zw;
    vec2 mv2 = mv23.xy;
    vec2 mv3 = mv23.zw;
    // branch lengths
	float length0 = sv0_l.w;
    float length1 = sv1_l.w;
    float length2 = sv2_l.w;
    float length3 = sv3_l.w;
	b_lengths.x = length0;
	b_lengths.y = length1;
	b_lengths.z	= length2;

    // sv vectors
	vec3 sv0 = sv0_l.xyz;
    vec3 sv1 = sv1_l.xyz;
    vec3 sv2 = sv2_l.xyz;
    vec3 sv3 = sv3_l.xyz;
    // amplitudes
	vec2 amp0 = wood_amplitudes.x * ( texture2D(branch_noise_tex, mv0 * mv_time * wood_frequencies.x).rg  * 2.0 - ONE2);
    vec2 amp1 = wood_amplitudes.y * ( texture2D(branch_noise_tex, mv1 * mv_time * wood_frequencies.y).rg  * 2.0 - ONE2);
    vec2 amp2 = wood_amplitudes.z * ( texture2D(branch_noise_tex, mv2 * mv_time * wood_frequencies.z).rg  * 2.0 - ONE2);
    vec2 amp3 = wood_amplitudes.w * ( texture2D(branch_noise_tex, mv3 * mv_time * wood_frequencies.w).rg  * 2.0 - ONE2);
    // apply animation to the vertex.
	//--------------------------------------------------------------------------------------
	
	vec3 tv;
	vec3 center;
	vec3 centerB = vec3(0.0, 0.0, 0.0);
	vec3 corr_r, corr_s;
	vec2 fu, fu_deriv, s,d;
	bs = sv0;
	br = rv0;
	bt = cross(br,bs);
	if (x_vals.x>0.0){
		corr_r = vec3(0.0);
		corr_s = vec3(0.0);
		// find t vector
		tv	= cross(rv0,sv0);
		// calc wind prebend offset
		amp0.x += dot(rv0, wind_direction) * wind_strength;
		amp0.y += dot(sv0, wind_direction) * wind_strength;

		// find branch origin
		center = centerB + x_vals.x * length0 * tv;
		// bend function
		fu	= xvals_f.x	* amp0;
		fu_deriv = xvals_deriv.x / length0 * amp0 ;
		if (abs(fu_deriv.x - 0.0)<EPSILON){ fu_deriv.x = EPSILON;}
		if (abs(fu_deriv.y - 0.0)<EPSILON){ fu_deriv.y = EPSILON;}
			s = sqrt(ONE2+fu_deriv*fu_deriv);
			d = fu / fu_deriv * (s - ONE2);
			corr_s = (tv + sv0*fu_deriv.x)/s.x * d.x;
			corr_r = (tv + rv0*fu_deriv.y)/s.y * d.y;
		
		//recalculate coord system of actual branch 
		bt  = normalize(tv + rv0*fu_deriv.y + sv0*fu_deriv.x);
		br	= normalize(rv0 - tv*fu_deriv.y);
		bs	= normalize(sv0 - tv*fu_deriv.x);
		// bend the center point
		centerB =  center + fu.x * sv0 + fu.y * rv0 - (corr_s+corr_r);
	}
    if (x_vals.y>0.0){
		corr_r = vec3(0.0);
		corr_s = vec3(0.0);
	    // bend branch system according to the parent branch bending
		sv1 = sv1.x * bs + sv1.y * br + sv1.z * bt;
        rv1 = rv1.x * bs + rv1.y * br + rv1.z * bt;
        //...
		tv	= cross(rv1,sv1);
		// calc wind prebend offset
		amp1.x += dot(rv1, wind_direction) * wind_strength;
		amp1.y += dot(sv1, wind_direction) * wind_strength;

        center		= centerB + x_vals.y * length1 * tv;
        fu			= xvals_f.y	 * amp1;
        fu_deriv	= xvals_deriv.y / length1 * amp1 ;
		if (abs(fu_deriv.x - 0.0)<EPSILON){ fu_deriv.x = EPSILON;}
		if (abs(fu_deriv.y - 0.0)<EPSILON){ fu_deriv.y = EPSILON;}
			s = sqrt(ONE2+fu_deriv*fu_deriv);
			d = fu / fu_deriv * (s - ONE2);
			corr_s = (tv + sv1*fu_deriv.x)/s.x * d.x;
			corr_r = (tv + rv1*fu_deriv.y)/s.y * d.y;
		
        bt  = normalize(tv + rv1*fu_deriv.y + sv1*fu_deriv.x);
        br	= normalize(rv1 - tv*fu_deriv.y);
        bs	= normalize(sv1 - tv*fu_deriv.x);

		centerB =  center + fu.x * sv1 + fu.y * rv1 - (corr_s+corr_r);
	}

	if (x_vals.z>0.0){
		corr_r = vec3(0.0);
		corr_s = vec3(0.0);
	    // bend branch system according to the parent branch bending
		sv2 = sv2.x * bs + sv2.y * br + sv2.z * bt;
        rv2 = rv2.x * bs + rv2.y * br + rv2.z * bt;
        //...
		tv	= cross(rv2,sv2);
		// calc wind prebend offset
		amp2.x += dot(rv2, wind_direction) * wind_strength;
		amp2.y += dot(sv2, wind_direction) * wind_strength;

        center		= centerB + x_vals.z * length2 * tv;
        fu			= xvals_f.z * amp2;
        fu_deriv	= xvals_deriv.z / length2 * amp2 ;
		if (abs(fu_deriv.x - 0.0)<EPSILON){ fu_deriv.x = EPSILON;}
		if (abs(fu_deriv.y - 0.0)<EPSILON){ fu_deriv.y = EPSILON;}
			s = sqrt(ONE2+fu_deriv*fu_deriv);
			d = fu / fu_deriv * (s - ONE2);
			corr_s = (tv + sv2*fu_deriv.x)/s.x * d.x;
			corr_r = (tv + rv2*fu_deriv.y)/s.y * d.y;
		
        bt  = normalize(tv + rv2*fu_deriv.y + sv2*fu_deriv.x);
        br	= normalize(rv2 - tv*fu_deriv.y);
        bs	= normalize(sv2 - tv*fu_deriv.x);
        centerB =  center + fu.x * sv2 + fu.y * rv2 - (corr_s+corr_r);
    }

	if (x_vals.w>0.0){
		corr_r = vec3(0.0);
		corr_s = vec3(0.0);

		sv3 = sv3.x * bs + sv3.y * br + sv3.z * bt;
        rv3 = rv3.x * bs + rv3.y * br + rv3.z * bt;
        tv	= cross(rv3,sv3);
		// calc wind prebend offset
		amp3.x += dot(rv3, wind_direction) * wind_strength;
		amp3.y += dot(sv3, wind_direction) * wind_strength;

        center		= centerB + x_vals.w * length3 * tv;
        fu			= xvals_f.w	 * amp3;
        fu_deriv	= xvals_deriv.w/ length3 * amp3 ;
		if (abs(fu_deriv.x - 0.0)<EPSILON){ fu_deriv.x = EPSILON;}
		if (abs(fu_deriv.y - 0.0)<EPSILON){ fu_deriv.y = EPSILON;}
			s = sqrt(ONE2+fu_deriv*fu_deriv);
			d = fu / fu_deriv * (s - ONE2);
			corr_s = (tv + sv3*fu_deriv.x)/s.x * d.x;
			corr_r = (tv + rv3*fu_deriv.y)/s.y * d.y;
		
        bt  = normalize(tv + rv3*fu_deriv.y + sv3*fu_deriv.x);
        br	= normalize(rv3 - tv*fu_deriv.y);
        bs	= normalize(sv3 - tv*fu_deriv.x);
        centerB =  center + fu.x * sv3 + fu.y * rv3 - (corr_s+corr_r);
    }
	
	
	//normal_vs	= bs;
	//tangent_vs	= bt;
	tangent_vs	 = tangent.x * bt + tangent.y * bs + tangent.z * br;
	normal_vs	 = normal.x  * bt + normal.y  * bs + normal.z  * br;
	//tangent_vs = normalize(tangent_vs);
	//normal_vs = normalize(normal_vs);
	//binormal = cross(tangent_vs, normal_vs);
	//tangent_vs = cross(normal_vs, binormal);
	position = centerB;
}



void animateLeafVertex(in vec3 origin, inout vec3 position, inout vec3 o_normal, inout vec3 o_tangent)
{
	// SCALE DOWN
	/*position *= 0.06;
	position += origin;
	*/
	// SAMPLE TURBULENT WIND FIELD
		// sample position
	vec2 samplePosition = wind_direction.xz * time * leaf_frequency * 0.01;


	vec2 texcoord = texCoords0.xy;

	const float scale = 1.5;
	vec3 amplitude = texture2D(leaf_noise_tex, origin.xz * scale).xyz * 2.0 - 1.0;
	amplitude += texture2D(leaf_noise_tex, origin.yz * scale - vec2(samplePosition.x,0)).xyz * 2.0 - 1.0;
	amplitude += texture2D(leaf_noise_tex, origin.xy * scale - vec2(samplePosition.y,0)).xyz * 2.0 - 1.0;

	amplitude *= leaf_amplitude;
	//float scaleL = 0.04;

	vec3 bitangent = cross(o_normal, o_tangent);

	// rotate tangent & binormal
	o_tangent = normalize ( o_tangent + o_normal*amplitude.x	);
	o_normal  = normalize ( cross(o_tangent, bitangent)			);
	o_normal  = normalize ( o_normal + bitangent*amplitude.y	);
	o_tangent = normalize ( cross (bitangent, o_normal	)		);
	
	

	//position = origin + scaleL * ( texCoords0.x * o_tangent + texCoords0.y * bitangent);

	/*


	vec3 normal_orig = normal;
	float rotationZ_offset = amplitude.x*(texcoord.x-0.5);
	position += rotationZ_offset * normal;

	normal = normal_orig + tangent*(rotationZ_offset/0.04406305);
	normal = normalize(normal);

	tangent = tangent + normal_orig*(rotationZ_offset/0.04406305);
	tangent = normalize(tangent);

	float rotationX_offset = amplitude.y*(texcoord.y)*0.5;
	position += rotationX_offset * tangent;

	vec3 bitangent = cross(normal, tangent);
	tangent -= bitangent*(rotationX_offset/0.1727);
	tangent = normalize(tangent);

	float n_offset = amplitude.z * (texcoord.y)*0.5;
	position += n_offset * normal;

	normal -= bitangent * (n_offset/0.1727);
	normal = normalize(normal);
	/*

	vec3 windSamplePosition = origin + wind_direction*leaf_frequency*time*0.01;
	
	
	vec3 xzDir = texture2D(leaf_noise_tex, windSamplePosition.xz).xyz * 2.0 - 1.0;
	vec3 yzDir = texture2D(leaf_noise_tex, windSamplePosition.yz).xyz * 2.0 - 1.0;
	vec3 xyDir = texture2D(leaf_noise_tex, windSamplePosition.xy).xyz * 2.0 - 1.0;

	// weight directions by wind direction
	// TODO
	vec3 leafDirection = normalize(	xzDir*(1-wind_direction.y) +
									yzDir*(1-wind_direction.x) +
									xyDir*(1-wind_direction.z));
	
	// rotate normal/tangent/binormal


	tangent = leafDirection;
	normal = cross(tangent, binormal);
	binormal = cross(normal, tangent);
	

	
	/*
	//vec2 looksvposition = wind_direction.xz * TimeOffsetLeaves;
	vec2 looksvposition = wind_direction.xz * wind_strength;
	
	
	vec3 amp =  texture2D(leaf_noise_tex, position.xz*0.1).xyz* 2.0 - 1.0;
		 amp += texture2D(leaf_noise_tex, position.yz*0.1-vec2(looksvposition.x,0)).xyz* 2.0 - 1.0;
		 amp += texture2D(leaf_noise_tex, position.xy*0.1-vec2(looksvposition.y,0)).xyz* 2.0 - 1.0;
	
	// ======================================================
	// tangent deformation
	// ======================================================
	vec3 nondef_normal = normal;
	float rotoffset = leaf_amplitude*amp.x*(texcoord.x-0.5);
	position += rotoffset*normal;
	
	normal = nondef_normal + tangent*(rotoffset/0.04406305);
	normal = normalize(normal);
	
	tangent = tangent + nondef_normal*(rotoffset/0.04406305);
	tangent = normalize(tangent);
	
	//------------------------------------------------------------------------------------------------
	
	vec3 tanoffset = leaf_amplitude*amp.y*(1 - texcoord.y)*0.5;
	position += tanoffset*tangent;
	
	vec3 bitangent = cross(normal,tangent);
	

	tangent -= bitangent*(tanoffset/0.1727);
	tangent = normalize(tangent);
	
	vec3 noffset = leaf_amplitude*amp.z*(1 - texcoord.y)*0.5;
	position += noffset*normal;
	
	normal -= bitangent*(noffset/0.1727);
	normal = normalize(normal);
	
*/	
}

void main()
{
    vec3 vertex = gl_Vertex.xyz;
	normal_vs = normal;
	tangent_vs = tangent;
    //vec3 norm	= normal;
    //vec3 tang	= tangent;
    //float bi	= branch_index;
    //vec4 x		= x_vals;
	
	vec3 leafOrigin = vec3(0.0, 0.0, 0.0);
	
	animateBranchVertex(leafOrigin);
	animateLeafVertex(leafOrigin, vertex, normal_vs, tangent_vs);
	
	vec3 bitangent = cross(normal_vs, tangent_vs);
	vertex = leafOrigin + (vertex.x*bitangent + vertex.y*tangent_vs);
	normal_vs = gl_NormalMatrix * normal_vs;
	tangent_vs = gl_NormalMatrix * tangent_vs;
	
	vPos = gl_ModelViewMatrix * vec4(vertex,1.0);
    gl_Position = vPos;
	gl_FrontColor = gl_Color;

}

