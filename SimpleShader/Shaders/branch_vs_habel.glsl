#


uniform sampler2DRect	branch_data_tex;
uniform sampler2D		branch_turbulence_tex;
uniform float			time;
attribute vec3			normal;
attribute vec3			tangent;
attribute vec4			x_vals;
attribute float			branch_index;

varying vec4		normal_vs;
varying vec4		tangent_vs;

vec4 pow4(vec4 vector, float exponent){
	return vec4(pow(vector.x,exponent), pow(vector.y,exponent), pow(vector.z,exponent), pow(vector.w,exponent) );
}

void animateVertexByHierarchy(inout vec3 position, inout vec3 normal,inout vec3 tangent, in vec4 xvals, in float branchindex)
{
    float AmplitudeWood = 1.0;
	//vec3 animated_vertex = position;
	
	//function for alpha = 0.1
	//vec4 xvals_f = 0.3326*pow4(xvals,2.0) + 0.398924*pow4(xvals,4.0);
	//vec4 xvals_deriv = 0.665201*xvals + 1.5957*pow4(xvals,3.0);
	
	//function for alpha = 0.2
	vec4 xvals_f = 0.374570*pow4(xvals,2.0) + 0.129428*pow4(xvals,4.0);
	vec4 xvals_deriv = 0.749141*xvals + 0.517713*pow4(xvals,3.0);
	
	//vec4 xvals_f = xvals;
	//vec4 xvals_deriv = 1.0;
	
	
	//vec4 xvals_f = xvals*xvals+ xvals*xvals*xvals*xvals;
	//vec4 xvals_deriv = xvals+ xvals*xvals*xvals;
				
	// get motionVectors from branch data texture
	vec4 motionVectors = texture2DRect(branch_data_tex, vec2(1.5, branchindex));
	
	
	// get coord systems from branch data texture
	vec4 sv0_l	= texture2DRect(branch_data_tex, vec2(2.5, branchindex));
	vec4 sv1_l	= texture2DRect(branch_data_tex, vec2(3.5, branchindex));
	vec4 sv2_l	= texture2DRect(branch_data_tex, vec2(4.5, branchindex));
	vec4 sv3_l	= texture2DRect(branch_data_tex, vec2(5.5, branchindex));
	vec4 rv0_c	= texture2DRect(branch_data_tex, vec2(6.5, branchindex));
	vec4 rv1_c	= texture2DRect(branch_data_tex, vec2(7.5, branchindex));
	vec4 rv2_c	= texture2DRect(branch_data_tex, vec2(8.5, branchindex));
	vec4 rv3_c	= texture2DRect(branch_data_tex, vec2(9.5, branchindex));
	
	// branch lenghts
	float length0 = sv0_l.w;
	float length1 = sv1_l.w;
	float length2 = sv2_l.w;
	float length3 = sv3_l.w;
	
	// sv vectors
	vec3 sv0 = sv0_l.xyz;
	vec3 sv1 = sv1_l.xyz;
	vec3 sv2 = sv2_l.xyz;
	vec3 sv3 = sv3_l.xyz;
	// rv vectors 
	vec3 rv0 = rv0_c.xyz;
	vec3 rv1 = rv1_c.xyz;
	vec3 rv2 = rv2_c.xyz;
	vec3 rv3 = rv3_c.xyz;
	
	// amplitudes
	vec2 amp = 10.0*vec2((sin(time)+1.0)/2.0, 0.0);
	vec2 amp0 = 10.0  * amp;//*texture2D(branch_turbulence_tex, vec2(TimeOffsetWood*0.25	, motionVectors.x)).xy);
	vec2 amp1 = 1.0  * amp;//*texture2D(branch_turbulence_tex, vec2(TimeOffsetWood*1	, motionVectors.y)).xy);
	vec2 amp2 = 0.7  * amp;//*texture2D(branch_turbulence_tex, vec2(TimeOffsetWood* 2	, motionVectors.z)).xy);
	vec2 amp3 = 0.45 * amp;//*texture2D(branch_turbulence_tex, vec2(TimeOffsetWood*4	, motionVectors.w)).xy);
	
	// apply animation to the vertex.
	//--------------------------------------------------------------------------------------
	
	if(xvals.x > 0.0) {
		vec3 twig	= cross(sv0,rv0);
		float fu_s	= xvals_f.x	 * amp0.x;
		float fu_r	= xvals_f.x	 * amp0.y;
		float fu_s_deriv = xvals_deriv.x * amp0.x / length0;
		float fu_r_deriv = xvals_deriv.x * amp0.y / length0;
	
		position = position + fu_s * sv0 + fu_r * rv0; 
		
		float s_s = sqrt(1.0+fu_s_deriv*fu_s_deriv);
		float d_s = fu_s/fu_s_deriv*(s_s-1.0);
		vec3 corr_s = (twig + sv0*fu_s_deriv)/s_s * d_s;
		
		float s_r = sqrt(1.0+fu_r_deriv*fu_r_deriv);
		float d_r = fu_r/fu_r_deriv*(s_r-1.0);
		vec3 corr_r = (twig + rv0*fu_r_deriv)/s_r * d_r;

		position = position - (corr_s+corr_r);
	}
	/*
	if(xvals.y > 0.0) {
		vec3 twig = cross(sv1,rv1);
		float fu_s = xvals_f.y*AmplitudeWood*amp1.x;
		float fu_r = xvals_f.y*AmplitudeWood*amp1.y;
		float fu_s_deriv = xvals_deriv.y*AmplitudeWood*amp1.x/length1;
		float fu_r_deriv = xvals_deriv.y*AmplitudeWood*amp1.y/length1;
	
		position +=  fu_s*sv1+fu_r*rv1; 
		
		float d_s = fu_s/fu_s_deriv*(sqrt(1+fu_s_deriv*fu_s_deriv)-1);
		vec3 corr_s = (twig + sv1*fu_s_deriv)*1/sqrt(1+fu_s_deriv*fu_s_deriv) *d_s;
		
		float d_r = fu_r/fu_r_deriv*(sqrt(1+fu_r_deriv*fu_r_deriv)-1);
		vec3 corr_r = (twig + rv1*fu_r_deriv)*1/sqrt(1+fu_r_deriv*fu_r_deriv) *d_r;
		
		position -= (corr_s+corr_r);
	}	
	if(xvals.z > 0.0) {	
		vec3 twig = cross(sv2,rv2);
		float fu_s = xvals_f.z*AmplitudeWood*amp2.x;
		float fu_r = xvals_f.z*AmplitudeWood*amp2.y;
		float fu_s_deriv = xvals_deriv.z*AmplitudeWood*amp2.x/length2;
		float fu_r_deriv = xvals_deriv.z*AmplitudeWood*amp2.y/length2;
	
		position +=  fu_s*sv2+fu_r*rv2; 
		
		float d_s = fu_s/fu_s_deriv*(sqrt(1+fu_s_deriv*fu_s_deriv)-1);
		vec3 corr_s = (twig + sv2*fu_s_deriv)*1/sqrt(1+fu_s_deriv*fu_s_deriv) *d_s;
		
		float d_r = fu_r/fu_r_deriv*(sqrt(1+fu_r_deriv*fu_r_deriv)-1);
		vec3 corr_r = (twig + rv2*fu_r_deriv)*1/sqrt(1+fu_r_deriv*fu_r_deriv) *d_r;
		
		position -= (corr_s+corr_r);
	}
	if(xvals.w > 0.0) {
		vec3 twig = cross(sv3,rv3);
		float fu_s = xvals_f.w*AmplitudeWood*amp3.x;
		float fu_r = xvals_f.w*AmplitudeWood*amp3.y;
		float fu_s_deriv = xvals_deriv.w*AmplitudeWood*amp3.x/length3;
		float fu_r_deriv = xvals_deriv.w*AmplitudeWood*amp3.y/length3;
	
		position +=  fu_s*sv3+fu_r*rv3; 
		
		float d_s = fu_s/fu_s_deriv*(sqrt(1+fu_s_deriv*fu_s_deriv)-1);
		vec3 corr_s = (twig + sv3*fu_s_deriv)*1/sqrt(1+fu_s_deriv*fu_s_deriv) *d_s;
		
		float d_r = fu_r/fu_r_deriv*(sqrt(1+fu_r_deriv*fu_r_deriv)-1);
		vec3 corr_r = (twig + rv3*fu_r_deriv)*1/sqrt(1+fu_r_deriv*fu_r_deriv) *d_r;
		
		position -= (corr_s+corr_r);
	}*/
	tangent = normalize(tangent);
	normal  = normalize(normal);	
}

void animateLeafVertex(inout vec3 position, inout vec3 normal, inout  vec3 tangent, in vec2 texcoord, in float branchindex)
{
	/*vec2 looksvposition = vec2(WindDirection.x, WindDirection.z) * TimeOffsetLeaves;
	
	
	vec3 amp = texture2D(NoiseSampler, position.xz*0.1).xyz* 2.0 - 1.0;
		amp += texture2D(NoiseSampler, position.yz*0.1-vec2(looksvposition.x,0)).xyz* 2.0 - 1.0;
		amp += texture2D(NoiseSampler, position.xy*0.1-vec2(looksvposition.y,0)).xyz* 2.0 - 1.0;
	
	vec3 nondef_normal = normal;
	float rotoffset = AmplitudeLeaves*amp.x*(texcoord.x-0.5);
	position += rotoffset*normal;
	
	normal = nondef_normal + tangent*(rotoffset/0.04406305);
	normal = normalize(normal);
	
	tangent = tangent + nondef_normal*(rotoffset/0.04406305);
	tangent = normalize(tangent);
	
	//------------------------------------------------------------------------------------------------
	
	vec3 tanoffset = AmplitudeLeaves*amp.y*(1 - texcoord.y)*0.5;
	position += tanoffset*tangent;
	
	vec3 bitangent = cross(normal,tangent);
	

	tangent -= bitangent*(tanoffset/0.1727);
	tangent = normalize(tangent);
	
	vec3 noffset = AmplitudeLeaves*amp.b*(1 - texcoord.y)*0.5;
	position += noffset*normal;
	
	normal -= bitangent*(noffset/0.1727);
	normal = normalize(normal);
	
	*/
}

void main()
{
	vec3 vertex = gl_Vertex.xyz;
	vec3 norm = normal;
	vec3 tang = tangent;
	animateVertexByHierarchy( vertex, norm, tang, x_vals, branch_index+0.5);
	//vec4 pokus = vec4(2.0, 2.0, 2.0, 2.0);
	//pokus = pow4(pokus, 2);

	normal_vs = vec4(norm, 0.0);
	tangent_vs = vec4(tang, 0.0);
	vec4 color = vec4 (vertex, 1.0);//texture2DRect(branch_data_tex, vec2(2.5, branch_index));
	gl_FrontColor = color;//*(sin(time)+1.0)/2.0;//gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;//vec4(vertex,1.0);
}
