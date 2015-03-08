#version 120
#define SHADOW_TRESHOLD 0.0005

uniform sampler2D color_texture;
uniform sampler2D frontDecalMap;
uniform sampler2D frontNormalMap;
uniform sampler2D frontTranslucencyMap;
uniform sampler2D frontHalfLife2Map;
uniform sampler2D backDecalMap;
uniform sampler2D backNormalMap;
uniform sampler2D backTranslucencyMap;
uniform sampler2D backHalfLife2Map;
uniform sampler2D seasonMap;
uniform sampler2DShadow shadowMap;
uniform int		shadowMappingEnabled;
varying vec4	lightSpacePosition;
vec4 lpos;
uniform	float season;
uniform float transition_control;
//uniform float MultiplyAmbient;
//uniform float MultiplyDiffuse;
//uniform float MultiplySpecular;
//uniform float MultiplyTranslucency;
//uniform float ReduceTranslucencyInShadow;

uniform float MultiplyAmbient			  ;
uniform float MultiplyDiffuse			  ;
uniform float MultiplySpecular			  ;
uniform float MultiplyTranslucency		  ;
uniform float ReduceTranslucencyInShadow  ;
uniform float shadow_intensity			  ;
uniform vec3  LightDiffuseColor			  ;

uniform vec3	colorVariance;

uniform float	gauss_shift;
uniform float	gauss_steep;
uniform float	gauss_weight;

uniform vec2			window_size;
#define SQRT6  0.40824829046386301636621401245098
#define SQRT2  0.70710678118654752440084436210485
#define SQRT3  0.57735026918962576450914878050196
#define SQRT23 0.81649658092772603273242802490196	

varying vec3			normal_vs;
varying vec3			tangent_vs;
varying vec4			vPos;
varying vec3			o_normal;
varying vec3			ts_viewDir_v;
varying vec3			ts_lightDir_v;

varying float			instanceSpecificNumber;
varying float			leafSpecificNumber;
								  
vec3			ts_viewDir		= normalize(ts_viewDir_v);
vec3			ts_lightDir		= normalize(ts_lightDir_v);

//--------------------------------------------------------------------------------------
// Includes and Defines
//--------------------------------------------------------------------------------------


#define ECCENTRICITY	0.2
#define COMP_ECC 		(ECCENTRICITY * ECCENTRICITY - 1.0)
#define ROLLOFF			0.3

#define REF_INDEX 1.3 	//1.3
#define ROUGHNESS 0.13	//0.13


//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------
const float infinity = 999999999;
/*
float getDepth(vec2 coords){
	if (clamp(coords.xy, 0.0, 1.0)!= coords.xy){
		return infinity; // infinity
	}

	float depth =  texture2D(shadowMap, coords.xy).x;
	if (depth>=1.0){
		return infinity;
	}
	return depth;
}
*/

float getRoughness(float cosa, float m)
{
	
	return (1.0 / (pow(m, 2.0) * pow(cosa, 4.0))) * exp(-((1.0 / pow(cosa, 2.0) - 1.0) / pow(m, 2.0)));
	//return (1.0 / (pow(m, 2.0) * pow(cosa, 4.0))) * exp(-((1.0 / pow(cosa, 2.0) - 1.0) / pow(m, 2.0)));
}

float getShadowingAndMaskingTerm(vec3 N, vec3 H, vec3 V, vec3 L)
{
	float m = min(2.0 * dot(N, H) * dot(N, V) / dot(V, H), 2.0 * dot(N, H) * dot(N, L) / dot(V, H));
	return min(1.0, m);
}


float getFresnelTerm(float n, float c)
{
	float g = sqrt(n * n + c * c - 1.0);
	return 0.5 * (pow((g - c), 2.0) / pow((g + c), 2.0)) * (1.0 + pow((c * (g + c) - 1.0), 2.0) / pow((c * (g - c) + 1.0), 2.0));
}


float getPhongSpecularity(vec3 light_vector, vec3 view_vector, vec3 normal)
{
	vec3 half_vector = normalize(light_vector + view_vector);
	float specularterm = max(dot(normal, half_vector), 0.0);
	specularterm = pow(specularterm, 50.0) * 0.7;
	specularterm = max(specularterm, 0.0);
	return specularterm;
}


float getCookTorranceSpecularity(vec3 light_vector, vec3 view_vector, vec3 normal)
{
	float specularity = 0.0;

	float cosne = dot(view_vector, normal);	
	float cosln = dot(light_vector, normal);

	if( cosln > 0.0 )
	{
		vec3 wHalfAngleDir = normalize(light_vector + view_vector);

		float coseh = dot(wHalfAngleDir, view_vector);
		float cosnh = dot(wHalfAngleDir, normal);

		float Dd = (COMP_ECC + 1.0) / (1.0 + COMP_ECC * cosnh * cosnh);
		Dd = Dd * Dd;
		cosnh = 2.0 * cosnh;

		float Gg;
		if (cosne < cosln)
		{
			Gg = (cosne * cosnh < coseh) ? cosnh / coseh : 1.0 / cosne;
		}
		else 
		{
			Gg = (cosln * cosnh < coseh) ? cosln * cosnh / (coseh * cosne) : 1.0 / cosne;
		}

		// Fresnel calculation.
		coseh = 1.0 - coseh;
		coseh = coseh * coseh * coseh;
		float Ff = coseh + (1.0 - coseh) * ROLLOFF;

		// Make sure that the specularCoefficient doesn't become negative.
		specularity = max(Dd * Gg * Ff, 0.0);
	}
	
	return specularity;
}



float getModifiedCookTorranceSpecularity(in vec3 light_vector, in vec3 view_vector, in vec3 normal, in float refractionindex, in float roughness)
{

	float specularity = 0.0;
	vec3 H = normalize(light_vector + view_vector);
	float NdotL = dot(normal, light_vector);//*0.5+0.5;
	float HdotV = max(dot(H,view_vector), 0.00000001);
	float NdotH = max(dot(normal, H), 0.00000001);
	float NdotV = max(dot(normal, view_vector), 0.00000001);
	float fresnel = getFresnelTerm(refractionindex, HdotV);
	
	
	float rough = getRoughness(NdotH, roughness);
		
	if (NdotL>0.0){	
		//specularity = (rough / NdotL) * fresnel / 3.14;

		specularity = (rough / NdotL) *
			max(0.0, (getShadowingAndMaskingTerm(normal, H, view_vector, light_vector) / NdotV) * fresnel / 3.14);

		specularity = clamp(specularity, 0.0, 1.0);
	}
	return specularity; //specularity;
	
}

float getShadow(vec2 position, vec2 offset, float depth){
	return shadow2D(shadowMap, vec3(position+offset*0.001,depth-SHADOW_TRESHOLD)).r; 
}

float getShadowIntensity(vec4 sm_pos){
	float res = getShadow(sm_pos.xy, vec2(0.0, 0.0), sm_pos.z) * 2.0;
	res += getShadow(sm_pos.xy, vec2(1.0, 0.0), sm_pos.z);
	res += getShadow(sm_pos.xy, vec2(-1.0, 0.0), sm_pos.z);
	res += getShadow(sm_pos.xy, vec2(0.0, 1.0), sm_pos.z);
	res += getShadow(sm_pos.xy, vec2(0.0, -1.0), sm_pos.z);
	return res/6.0;
}



void colorize(out vec4 outColor, in vec3 normal, in vec3 tangent, in vec3 bitangent){
	// Texture lookups
	
	vec4 decal_color;
	vec4 normal_color;
	vec4 translucency_color;
	vec4 halflife2_color;
	vec4 cpvcolor		= vec4(1.0);
	vec4 other_cpvcolor	= vec4(1.0);
	const float k = 1.5;
	if (!gl_FrontFacing)
	{
		decal_color			= pow(	texture2D(frontDecalMap			, gl_TexCoord[0].xy ) , vec4(k));
		normal_color		=		texture2D(frontNormalMap		, gl_TexCoord[0].xy );
		translucency_color	= pow(	texture2D(frontTranslucencyMap	, gl_TexCoord[0].xy ) , vec4(k));
		halflife2_color		=		texture2D(frontHalfLife2Map		, gl_TexCoord[0].xy );
		
		//cpvcolor = IN.color1;
		//other_cpvcolor = IN.color2;
	}
	else
	{
		decal_color			= pow(	texture2D(backDecalMap			, gl_TexCoord[0].xy ) , vec4(k));
		normal_color		=		texture2D(backNormalMap			, gl_TexCoord[0].xy );
		translucency_color	= pow(	texture2D(backTranslucencyMap	, gl_TexCoord[0].xy ) , vec4(k));
		halflife2_color		=		texture2D(backHalfLife2Map		, gl_TexCoord[0].xy );
		//discard;
		//cpvcolor = IN.color2;
		//other_cpvcolor = IN.color1;
	}
	
	// SEASON COLOR
	vec2 seasonCoord = vec2(0.5, season + 0.2*leafSpecificNumber - 0.0001*instanceSpecificNumber);
	
	vec4 seasonColor =  texture2D(seasonMap, seasonCoord);
	if (seasonColor.a<0.5){
		discard;
	}
	decal_color.rgb += seasonColor.rgb;
	decal_color.rgb *= colorVariance.rgb;
	decal_color.a *= seasonColor.a;
	translucency_color.rgb += seasonColor.rgb;
	translucency_color.rgb *= colorVariance.rgb;
	// END SEASON COLOR
	
	// ALPHA TEST
	if (decal_color.a < 0.5)
	{
		discard;
	}
	decal_color.a = 1.0;
	// END ALPHA TEST


	// SHADOW MAPPING //
	float shadow = 1.0;
	if (shadowMappingEnabled>0){		
		shadow = (1.0 - (1.0 - getShadowIntensity(lightSpacePosition))*(shadow_intensity));
	}
	// END SHADOW MAPPING //

	// SHADING

	float translucency;
	float specularity = 0.0;
	float diffuse_term;
	
	// Calculate tangent space normal.
	vec3 ts_normal = normalize(normal_color.xyz * 2.0 - 1.0);
	
	// Calculate translucency intensity

	vec3 n = ts_normal;
	vec3 v = ts_viewDir; 
	if (!gl_FrontFacing)
	{
		// Calculate translucency intensity.
		
		translucency =	dot(ts_lightDir, vec3(-SQRT6, -SQRT2,  SQRT3)) * halflife2_color.x +
					    dot(ts_lightDir, vec3(-SQRT6,  SQRT2,  SQRT3)) * halflife2_color.y +
						dot(ts_lightDir, vec3( SQRT23,   0.0,  SQRT3)) * halflife2_color.z;
		translucency = max(translucency, 0.0);
		
		// Calculate specularity.
		
		ts_lightDir = -ts_lightDir;
		specularity = getModifiedCookTorranceSpecularity(ts_lightDir, v, n, REF_INDEX, ROUGHNESS);
	}
	else
	{
		// Calculate translucency intensity.
		translucency =	dot(ts_lightDir, vec3(-SQRT6, -SQRT2,  -SQRT3)) * halflife2_color.x +
						dot(ts_lightDir, vec3(-SQRT6,  SQRT2,  -SQRT3)) * halflife2_color.y +
						dot(ts_lightDir, vec3( SQRT23,   0.0,  -SQRT3)) * halflife2_color.z;
		translucency = max(translucency, 0.0);

		// Calculate specularity.
		specularity = 0.0;//getModifiedCookTorranceSpecularity(IN.direction_to_light, -IN.view_vector, ts_normal, RefractionIndex, Roughness);
		//ts_normal = -ts_normal;
	}

	// Calculate diffuse lighting.
	diffuse_term = max(0.0, dot(ts_lightDir, ts_normal));
	diffuse_term = min(diffuse_term, shadow);

	// Calculate lighting terms.
	
	vec3 translucency_in_light = translucency * other_cpvcolor.rgb * gl_LightSource[0].diffuse.rgb ;

	vec3 final_translucency = translucency_color.rgb * translucency_in_light * MultiplyTranslucency * (shadow);
	
	vec4 final_ambient = cpvcolor * gl_LightSource[0].ambient  * MultiplyAmbient;
	vec4 final_diffuse = diffuse_term * gl_FrontLightProduct[0].diffuse * MultiplyDiffuse;
	
	vec4 final_specular = specularity * shadow * gl_FrontLightProduct[0].diffuse * MultiplySpecular;
	//outColor = vec4(vec3(translucency),1.0);// * final_ambient.rgb + 0.0001*(final_diffuse.rgb + final_specular.rgb + final_translucency);	
	outColor.rgb = (decal_color.rgb * (final_ambient.rgb + final_diffuse.rgb) + final_specular.rgb + final_translucency); //
	//outColor.rgb = final_specular.rgb; 
	outColor.a = decal_color.a;

	
}


void main()
{	

	vec3 normal_v = normalize(normal_vs);
	vec3 tangent_v = normalize(tangent_vs);

	vec3 bitangent = cross(normal_v, tangent_v);
	vec4 color;

	colorize(color, normal_v, tangent_v, bitangent);

	//color = 0.5*texture2D(frontDecalMap, gl_TexCoord[0].yx) + 0.2*texture2D(frontTranslucencyMap, gl_TexCoord[0].yx + ts_lightDir.xy );
	color.a = gl_Color.a;
	//if (texture2D(frontDecalMap, gl_TexCoord[0].yx).a<0.9) discard;
	//float control = clamp (1.0 - 2.0*abs(0.25 - transition_control), 0.0,1.0);
	
	// TODO: move into vertex shader (no need to compute per pixel...)
	/*
	float s = gauss_steep; // steepness
	float m = gauss_shift; // shift
	float a = 1.0/(s*sqrt(2.0*3.14159265));
	float b = m;
	float c = s;
	float gaussianCurve = a*exp(-(transition_control-b)*(transition_control-b)/(2*c*c));
	
	color.rgb = pow(color.rgb, vec3(1.0 + gauss_weight*gaussianCurve));
	*/
	gl_FragData[0] = color;
	//gl_FragData[0] = color;
	gl_FragData[1] = color * vec4(0.3, 0.3, 0.3, 1.0);
	//gl_FragData[1] =vec4(0.0, 0.0, 0.0, 1.0);
	
}
