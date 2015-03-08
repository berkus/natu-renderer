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
//uniform sampler2D		branch_turbulence_tex;
uniform float			branch_count;
uniform float			time;
uniform float			time_offset;
uniform sampler2D		data_tex;
uniform sampler2D		branch_noise_tex;
uniform sampler2D		leaf_noise_tex;
uniform vec3			wind_direction;
uniform float			wind_strength;
uniform vec4			wood_amplitudes;
uniform vec4			wood_frequencies;

attribute vec3			normal;
attribute vec3			tangent;
attribute vec4			x_vals;
attribute float			branch_index;
attribute vec2			texCoords0;
varying vec2			mv_v;

float				time_faster;
varying vec3		normal_vs  ;
varying vec3		normal_v   ;
varying vec3		tangent_vs ;
varying float		level;

varying vec2		b0_origin;
varying vec3		b1_origin;
varying vec2		b2_origin;

varying float		branchID;
vec4 color;	
varying vec4 vPos;
vec3					oVec=vec3(1.0, 1.0, 1.0);

vec4 ND2TC(in vec4 position){
	vec4 o = position * 0.5 + vec4(0.5);
	o.w = 1.0;
	return o;
}

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
    float mv_time = (time+time_offset) * 0.01;
   
    //function for alpha = 0.1
	//vec4 xvals_f = 0.3326*pow4(xvals,2.0) + 0.398924*pow4(xvals,4.0);
	//vec4 xvals_deriv = 0.665201*xvals + 1.5957*pow4(xvals,3.0);
	
	//function for alpha = 0.2
	vec4 xvals_f = 0.374570*x_vals*x_vals + 0.129428*x_vals*x_vals*x_vals*x_vals;
    vec4 xvals_deriv = 0.749141*x_vals + 0.517713*x_vals*x_vals*x_vals;


	
	
	// get coord systems from branch data texture
	branchID = branch_index/branch_count;
	vec4 sv0_l	= texture2D(data_tex, vec2(2.5/texCols, branchID));
    vec4 sv1_l	= texture2D(data_tex, vec2(3.5/texCols, branchID));
    vec4 sv2_l	= texture2D(data_tex, vec2(4.5/texCols, branchID));
    vec4 sv3_l	= texture2D(data_tex, vec2(5.5/texCols, branchID));
    vec3 rv0	= texture2D(data_tex, vec2(6.5/texCols, branchID)).xyz;
    vec3 rv1	= texture2D(data_tex, vec2(7.5/texCols, branchID)).xyz;
    vec3 rv2	= texture2D(data_tex, vec2(8.5/texCols, branchID)).xyz;
    vec3 rv3	= texture2D(data_tex, vec2(9.5/texCols, branchID)).xyz;
    vec2 bis    = texture2D(data_tex, vec2(16.5/texCols, branchID)).xy; 
    branchID = (max(bis.x,bis.y)+ 0.5)/branch_count;
	// motion vectors
	vec4 mv01 = texture2D(data_tex, vec2(0.5/texCols, branchID));
    vec4 mv23 = texture2D(data_tex, vec2(1.5/texCols, branchID));
    vec2 mv0 = mv01.xy;
    vec2 mv1 = mv01.zw;
	mv_v = mv1;
    vec2 mv2 = mv23.xy;
    vec2 mv3 = mv23.zw;
    // branch lengths
	float length0 = sv0_l.w;
    float length1 = sv1_l.w;
    float length2 = sv2_l.w;
    float length3 = sv3_l.w;
    // sv vectors
	vec3 sv0 = sv0_l.xyz;
    vec3 sv1 = sv1_l.xyz;
    vec3 sv2 = sv2_l.xyz;
    vec3 sv3 = sv3_l.xyz;
    // amplitudes
	//vec2 amp0 = wood_amplitudes.x * ( texture2D(branch_noise_tex, mv0 * mv_time * wood_frequencies.x).rg  * 2.0 - ONE2);
    //vec2 amp1 = wood_amplitudes.y * ( texture2D(branch_noise_tex, mv1 * mv_time * wood_frequencies.y).rg  * 2.0 - ONE2);
    //vec2 amp2 = wood_amplitudes.z * ( texture2D(branch_noise_tex, mv2 * mv_time * wood_frequencies.z).rg  * 2.0 - ONE2);
    //vec2 amp3 = wood_amplitudes.w * ( texture2D(branch_noise_tex, mv3 * mv_time * wood_frequencies.w).rg  * 2.0 - ONE2);
    // apply animation to the vertex.
	//--------------------------------------------------------------------------------------
	
	vec3 tv;
	vec3 center;
	vec3 centerB = vec3(0.0, 0.0, 0.0);
	b0_origin = OS2ND(vec4(centerB,1.0)).xy;
	b1_origin = vec3(0.0);
	vec3 corr_r, corr_s;
	vec2 fu, fu_deriv, s,d;
	bs = sv0;
	br = rv0;
	bt = cross(br, bs);
	if (x_vals.x>0.0){
		level = 0.0;
		// level0
		// find t vector
		tv	= cross(rv0,sv0);
	
		// calc wind prebend offset
		//amp0.x += dot(rv0, wind_direction) * wind_strength;
		//amp0.y += dot(sv0, wind_direction) * wind_strength;

		// find branch origin
		centerB = centerB + x_vals.x * length0 * tv;
		// bend function
		//recalculate coord system of actual branch 
		bt  = tv ; //normalize(tv);
		br	= rv0; //normalize(rv0);
		bs	= sv0; //normalize(sv0);
		
		
	}
    if (x_vals.y>0.0){
		b1_origin = ND2TC(OS2ND(vec4(centerB,1.0))).xyz;
       
	    // bend branch system according to the parent branch bending
		sv1 = sv1.x * br + sv1.y * bs + sv1.z * bt;
        rv1 = rv1.x * br + rv1.y * bs + rv1.z * bt;
        //...
		tv	= cross(rv1,sv1);

        centerB		= centerB + x_vals.y * length1 * tv;
       
        bt  = tv ; //normalize(tv);
        br	= rv1; //normalize(rv1);
        bs	= sv1; //normalize(sv1);

		
    }

	if (x_vals.z>0.0){
		b2_origin = OS2ND(vec4(centerB,1.0)).xy;
	    // bend branch system according to the parent branch bending
		sv2 = sv2.x * br + sv2.y * bs + sv2.z * bt;
        rv2 = rv2.x * br + rv2.y * bs + rv2.z * bt;
        //...
		tv	= cross(rv2,sv2);
		// calc wind prebend offset
		

        centerB		= centerB + x_vals.z * length2 * tv;
        bt  = tv ; //normalize(tv );
        br	= rv2; //normalize(rv2);
        bs	= sv2; //normalize(sv2);
    }

	if (x_vals.w>0.0){
		sv3 = sv3.x * br + sv3.y * bs + sv3.z * bt;
        rv3 = rv3.x * br + rv3.y * bs + rv3.z * bt;
        tv	= cross(rv3,sv3);
		// calc wind prebend offset
		centerB		= centerB + x_vals.w * length3 * tv;
        bt  = tv ; //normalize(tv );
        br	= rv3; //normalize(rv3);
        bs	= sv3; //normalize(sv3);
    }
	
	vec3	offset = position.x*bs + position.y*br;
	position = centerB + offset;
	normal_vs = normalize(offset);
    tangent_vs	 = bt;
}

void main()
{
    vec3 vertex = gl_Vertex.xyz;
    animateBranchVertex(vertex);
	gl_TexCoord[0] = vec4(texCoords0, 0.0, 0.0);	
	//normal_vs = gl_NormalMatrix * normal_vs;
	//normal_vs  = normal;
	//tangent_vs = tangent;
	//normal_vs = -normal_vs;
	//normal_v = gl_NormalMatrix * normal_vs.xzy;
	//normal_vs = normalize(vertex);
	normal_vs = gl_NormalMatrix * normal_vs;
	tangent_vs = gl_NormalMatrix * tangent_vs;
	vPos = gl_ModelViewMatrix * vec4(vertex,1.0);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex,1.0);
	
}

