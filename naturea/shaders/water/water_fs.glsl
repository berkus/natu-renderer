#
//==============================================================================
//  Water FRAGMENT shader   
//  inspired by: http://forum.bonzaisoftware.com/viewthread.php?tid=10
//				 http://www.bonzaisoftware.com/water_tut.html#refl
//   
//               GLSL conversion of Michael Horsch water demo
//               http://www.bonzaisoftware.com/wfs.html
//
//               Converted by Mars_999
//               8/20/2005            
//==============================================================================
#define WAVES_SCALE 30.0

uniform sampler2D water_reflection;
uniform sampler2D water_refraction;
uniform sampler2D water_dudvmap;
uniform sampler2D water_depthmap;

uniform float time;
varying vec4 projectedVertex; //for projection


const vec4 waterColor = vec4(0.1,0.36,0.51,1.0);
void main(void)
{
	
	//get projective texcoords
	vec4 tmp = vec4(1.0 / projectedVertex.w);
	
	vec4 projCoord = projectedVertex * tmp;
	projCoord += vec4(1.0);
	projCoord *= vec4(0.5);
	//tmp = projCoord + fdist;
	projCoord = clamp(projCoord, 0.001, 0.999);
	vec2 texCoord = vec2(projCoord);

	//load reflection,refraction and depth texture
	vec2 dir = vec2(1.0, 1.0)*0.08;
	
	vec4 dudv = (texture2D(water_dudvmap, gl_TexCoord[0].xy * WAVES_SCALE + dir * time)/128.0)- vec4(1.0);

	vec2 shift1		= -clamp( -(texCoord + dudv.xy), 0.001, 0.999 );
	vec2 shift2		= clamp(texCoord - dudv.xy, 1.001 , 1.999);
	vec4 reflection = texture2D(water_reflection, shift1);
	vec4 refr		= texture2D(water_refraction, shift2);
	vec4 wdepth		= texture2D(water_depthmap, texCoord);

	//add reflection and refraction
	float depthFactor = 0.9;
	float frag_depth = gl_FragDepth; 
	float water_depth = wdepth.x;
	float factor = abs(frag_depth-water_depth)*depthFactor;
	float factor3 = factor*factor*factor;
	
	gl_FragData[0] = reflection*0.5 + (1-factor3)*refr + 0.5*factor3*waterColor;
	
	gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);
	
}