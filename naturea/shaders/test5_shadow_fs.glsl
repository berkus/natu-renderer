#version 120
#define SHADOW_TRESHOLD 0.0001
#define ALPHA_TRESHOLD 0.5
#define ONE2    vec2(1.0,1.0)
#define EPSILON 0.0001
#define EPSILONVEC vec2(EPSILON, EPSILON)
uniform sampler2D	colorMap;
uniform sampler2D	branch_noise_tex;
uniform sampler2D	leaf_noise_tex;
uniform sampler2D	normalMap;
uniform	sampler2D   dataMap;
uniform sampler2D	seasonMap;
uniform sampler2D	lod_data_tex;
uniform sampler2D	depthMap;
uniform float		season;
uniform float		time;
uniform	vec2		movementVectorA;
uniform	vec2		movementVectorB;

uniform vec4		wood_amplitudes;
uniform vec4		wood_frequencies;
uniform float		leaf_amplitude;
uniform float		leaf_frequency;

varying vec2		sliceDesc;

varying float		mv_time;
varying float		time_offset_v;

uniform float		branch_count;
uniform	float		farMnear;
varying vec3		v_wind_dir_ts;
uniform float		wind_strength;
uniform float		sliceCnt;
uniform float		sliceSetsCnt;
#define	texCols			3.0

const float infinity = 999999999;

void animateBranch(inout vec2 position, in float bid, in float time, in float offset, in float texCol, in float wood_a, in float wood_f, in vec3 wind_d, in float wind_s){
	vec2 mv;
	vec2 corr_s;
	vec2 corr_r;
	float x_val;
	vec2 amp;
	vec4 b_data0;
	vec4 b_data1;
	vec4 b_data2;
	vec4 b_data3;
	vec2 o;
	vec2 t; 
	vec3 r; 
	vec3 s; 
	float l;
	b_data0 = texture2D(lod_data_tex, vec2((0.5+offset)*texCol, bid));
	b_data1 = texture2D(lod_data_tex, vec2((1.5+offset)*texCol, bid));
	b_data2 = texture2D(lod_data_tex, vec2((2.5+offset)*texCol, bid));
	//b_data3 = texture2D(lod_data_tex, vec2((3.5+offset)*texCol, branchID));
	o = b_data0.xy;
	mv = b_data0.zw;
	r = b_data1.xyz;
	s = b_data2.xyz;
	t = cross(s,r).xy;
	l = b_data1.w;
		
	// get x value on the projected branch

	// naive solution = distance to projected origin / branch projected length
	// PROBLEMS:
	// - what about branches pointing to the observer? - projected length is near 0
	// - even pixels close to projected origin can be very far in terms of x
	vec2 distVector = abs(position-o);
	//offset = 0.0; //1.0 - length(t.xy);
	//x_val = min(1.0, offset + length(distVector)/l);
	x_val = min(1.0, length(distVector)/l);
	//color = vec4(x_val);
	vec2 w = vec2(dot(r, wind_d) * wind_s, dot(s, wind_d) * wind_s);
	amp = w + wood_a * ( texture2D(branch_noise_tex, mv * time * wood_f).rg  * 2.0 - ONE2);
	float xval2 = x_val*x_val;
	
	float fx = 0.374570*xval2 + 0.129428*xval2*xval2;
	float dx = 0.749141*x_val + 0.517713*xval2*x_val;

	vec2 fu			= vec2(fx)		* amp;
	vec2 fu_deriv	= vec2( dx / l) * amp ;
	// restrict fu_deriv != 0.0
	fu_deriv = max(fu_deriv, EPSILONVEC) + min(fu_deriv, EPSILONVEC);
	vec2 us = sqrt(ONE2+fu_deriv*fu_deriv);
	vec2 ud = fu / fu_deriv * (us - ONE2);
	corr_r = (t + r.xy*fu_deriv.x)/us.x * ud.x;
	corr_s = (t + s.xy*fu_deriv.y)/us.y * ud.y;
	// inverse deformation - must be aplyed in oposite direction
	position = position - ( fu.x * r.xy + fu.y * s.xy - (corr_r+corr_s) );
}

vec2 convert2TexCoords(in vec2 sliceCoords){
	return (clamp ( sliceCoords  , vec2(0.0, 0.0), ONE2 ) + sliceDesc) / vec2(sliceCnt,sliceSetsCnt);
}

void	main()
{	

	if (gl_Color.a==0.0){discard;}
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 cVar;
	vec4 seasonColor = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 normal;
	float leaf = 0.0;
	vec2 newPos;
	float mv_time = (time+time_offset_v) * 0.01;
	// get frag position
	vec2 position = gl_TexCoord[0].st;
	vec2 tpos	= clamp ( position + sliceDesc , sliceDesc, sliceDesc+vec2(1.0, 1.0) ) / vec2(sliceCnt,sliceSetsCnt);
	// get Level-1 branch
	float branchID = (texture2D(dataMap, tpos).r);
	float branchFlag = sign(branchID);
	branchID = abs(branchID);
	float offset = sliceDesc.y*texCols;
	float texCol = 1.0/(sliceSetsCnt*texCols);
	
	// level 1 deformation...
	if (branchID>(0.5)/branch_count){
		animateBranch(position, branchID, mv_time, offset, texCol, wood_amplitudes.y, wood_frequencies.y, v_wind_dir_ts, wind_strength);
	}
	// animate trunk
	animateBranch(position, 0.0, mv_time, offset, texCol, wood_amplitudes.x, wood_frequencies.x, v_wind_dir_ts, wind_strength);
	if (position.y>1.0){
		discard;
	}
	// is it a leaf-fragment? yes -> can animate leaf
	newPos = convert2TexCoords(position);
	bool isLeaf = sign(texture2D(dataMap, newPos).r)>=0.0;
	float NdotL;
	if (isLeaf){
		// leaf

		// distort...
		vec2 texCoordA = tpos+leaf_frequency*mv_time*movementVectorA*2.0;
		vec2 texCoordB = tpos+leaf_frequency*mv_time*movementVectorB*2.0;

		vec2 noise = ((texture2D(leaf_noise_tex, texCoordB).st + texture2D(leaf_noise_tex, texCoordA).st) - ONE2);
		vec2 newPosD = convert2TexCoords(position + leaf_amplitude*0.005*noise);
		// is the source fragment from branch? if so use original coords...
		if(sign(texture2D(dataMap, newPosD).r)>=0.0){
			newPos = newPosD;
		}
		color += texture2D(colorMap, newPos);
		if (color.a<ALPHA_TRESHOLD){
			discard;
		}
		normal = texture2D(normalMap, newPos);
		float leaf = (1.0/(1.0-0.004))*(normal.w-0.004);
		vec2 seasonCoord = vec2(0.5, season + 0.2*leaf - 0.0001*time_offset_v);		
		seasonColor = texture2D(seasonMap, seasonCoord);
		if (seasonColor.a<0.5){
			discard;
		}
	} else {
		//branch
		color += texture2D(colorMap, newPos);
		if (color.a<ALPHA_TRESHOLD){
			discard;
		}
	}
	// DITHER
	float treshold = gl_Color.a;
	if (treshold<1.0 && treshold>0.0){
		vec2 screen_pos = gl_FragCoord.xy*0.001;
		float noise = texture2D(branch_noise_tex, screen_pos).r;
		if (noise>treshold){
			discard;
		}
	}

	color.a *= gl_Color.a;
	float front = -1.0;
	if (gl_FrontFacing){
		front = 1.0;
	}

	float depth_tex = texture2D(depthMap, newPos).r;
	//gl_FragDepth = gl_FragCoord.z + front*(depth_tex-0.5)/(sliceCnt*(farMnear));
	
	gl_FragData[0] = color;
}
