#version 120

uniform sampler2D color_texture;
uniform sampler2D frontDecalMap;
uniform sampler2D frontNormalMap;
uniform sampler2D frontTranslucencyMap;
uniform sampler2D frontHalfLife2Map;
uniform sampler2D backDecalMap;
uniform sampler2D backNormalMap;
uniform sampler2D backTranslucencyMap;
uniform sampler2D backHalfLife2Map;

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

uniform vec2			window_size;
#define SQRT6  0.40824829046386301636621401245098
#define SQRT2  0.70710678118654752440084436210485
#define SQRT3  0.57735026918962576450914878050196
#define SQRT23 0.81649658092772603273242802490196	

varying vec4			vPos;
varying vec3			o_normal;
varying vec3			ts_viewDir_v;
varying vec3			ts_lightDir_v;

varying vec2			b0_origin;
varying vec3			b1_origin;
varying vec2			b2_origin;
varying vec3			b_lengths;
varying vec2			mv_v;

varying float			leafSpecificNumber;
varying float			branchID;

//uniform vec3			cam_dir;
//uniform vec3			cam_right;
//vec3					cam_up = cross(cam_dir, cam_right);

varying vec3			normal_vs;
varying vec3			tangent_vs;
vec3					ts_normal;


								  
vec3			ts_viewDir		= normalize(ts_viewDir_v);
vec3			ts_lightDir		= normalize(ts_lightDir_v);

//--------------------------------------------------------------------------------------
// Includes and Defines
//--------------------------------------------------------------------------------------


#define ECCENTRICITY	0.2
#define COMP_ECC 		(ECCENTRICITY * ECCENTRICITY - 1.0)
#define ROLLOFF			0.3

#define REF_INDEX 1.3 	//1.3
#define ROUGHNESS 0.2	//0.13


//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------

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



float getModifiedCookTorranceSpecularity(vec3 light_vector, vec3 view_vector, vec3 normal, float refractionindex, float roughness)
{
	
	/*
	float specularity = 0.0;	
	vec3 wHalfAngleDir = normalize(light_vector + view_vector);
	float fresnel = getFresnelTerm(refractionindex, dot(view_vector, wHalfAngleDir));
	float rough = getRoughness(dot(normal, wHalfAngleDir), roughness);
	specularity = (rough / dot(normal, light_vector)) *
		max(0.0, (getShadowingAndMaskingTerm(normal, wHalfAngleDir, view_vector, light_vector) / dot(normal, view_vector)) * fresnel / 3.14);
	return clamp(specularity, 0.0, 2.0);
	
	


	/*/

	float specularity = 0.0;
	vec3 H = normalize(-light_vector + view_vector);
	float NdotL = dot(normal, light_vector)*0.5+0.5;
	float VdotH = max(dot(view_vector, H), 0.00000001);
	float NdotH = max(dot(normal, H), 0.00000001);
	
	float fresnel = getFresnelTerm(refractionindex, VdotH);
	
	
	float rough = getRoughness(NdotH, roughness);
		
	if (NdotL>0.0){	
		specularity = (rough / NdotL) * fresnel / 3.14;
/*
		specularity = (rough / NdotL) *
			max(0.0, (getShadowingAndMaskingTerm(normal, wHalfAngleDir, view_vector, light_vector) / NdotV) * fresnel / 3.14);
*/
		specularity = clamp(specularity, 0.0, 1.0);
	}
	return specularity; //specularity;
	
}





void colorize(out vec4 outColor, in vec3 normal, in vec3 tangent, in vec3 bitangent){
	// Texture lookups.
	vec4 decal_color;
	vec4 normal_color;
	vec4 translucency_color;
	vec4 halflife2_color;
	vec4 cpvcolor		= vec4(1.0);
	vec4 other_cpvcolor	= vec4(1.0);
	const float k = 1.5;
	if (!gl_FrontFacing)
	{
		//decal_color			= pow(	texture2D(frontDecalMap			, gl_TexCoord[0].xy ) , vec4(k));
		decal_color			=		texture2D(frontDecalMap			, gl_TexCoord[0].xy );
		normal_color		=		texture2D(frontNormalMap		, gl_TexCoord[0].xy );
		}
	else
	{
		//decal_color			= pow(	texture2D(backDecalMap			, gl_TexCoord[0].xy ) , vec4(k));
		decal_color			=		texture2D(backDecalMap			, gl_TexCoord[0].xy );
		normal_color		=		texture2D(backNormalMap			, gl_TexCoord[0].xy );
		}

	
	// --- Alpha Testing ---

	if (decal_color.a < 0.5)
	{
		discard;
	}
    
	// --- End Alpha Testing ---


	// --- Shadow Mapping ---

//	float shadow_intensity;
//	vec3 shadow_color = getShadowColor(shadow_intensity, IN.shadowmap_lookup);
	
	// --- End Shadow Mapping ---


	// --- Lighting ---
	float diffuse_term;
	
	// Calculate tangent space normal.
	ts_normal = normalize(normal_color.xyz * 2.0 - 1.0);
	
	// Calculate translucency intensity.
	if (!gl_FrontFacing)
	{

	}
	else
	{
		
		ts_normal.z = -ts_normal.z;
	}

	// Calculate diffuse lighting.
	diffuse_term = max(0.0, dot(-ts_lightDir, ts_normal));
	diffuse_term = min(diffuse_term, shadow_intensity);

	// --- End Lighting ---
	
	
	// --- Combine terms for final output color ---
	
	vec4 final_ambient = decal_color * cpvcolor * gl_LightSource[0].ambient  * MultiplyAmbient;
	vec4 final_diffuse = decal_color * diffuse_term * gl_FrontLightProduct[0].diffuse * MultiplyDiffuse;
	
	outColor.rgb = final_ambient.rgb + final_diffuse.rgb; //
	//outColor.rgb = final_specular.rgb; 
	outColor.a = decal_color.a;
}


void main()
{	
	vec3 normal_v = normalize(normal_vs);
	vec3 tangent_v = normalize(tangent_vs);


	vec3 bitangent = cross(normal_v, tangent_v);
	vec4 color ;
	color = texture2D(frontDecalMap	, gl_TexCoord[0].xy );
	if (color.a<0.5){discard;}
	color.a = 1.0;
	//colorize(color, normal_v, tangent_v, bitangent);
	gl_FragData[0] = color;
	//vec3 normal;
	gl_FragData[1] = vec4(normal_v*0.5 + vec3(0.5) , 0.004+leafSpecificNumber);
	//gl_FragData[2] = vec4(b0_origin*0.5+vec2(0.5),b1_origin*0.5+vec2(0.5));
	gl_FragData[2] = vec4(branchID);
	
}
