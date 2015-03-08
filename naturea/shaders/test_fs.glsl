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

const float MultiplyAmbient=0.7;
const float MultiplyDiffuse=1.0;
const float MultiplySpecular=0.3;
const float MultiplyTranslucency=0.9;
const float ReduceTranslucencyInShadow=0.9;
const float shadow_intensity = 1.0;
const vec3  LightDiffuseColor = vec3(0.2, 0.2, 0.2);


#define SQRT6  0.40824829046386301636621401245098
#define SQRT2  0.70710678118654752440084436210485
#define SQRT3  0.57735026918962576450914878050196
#define SQRT23 0.81649658092772603273242802490196	

varying vec3			normal_vs_v;
varying vec3			tangent_vs_v;
varying vec4			vPos;
varying vec3			o_normal;
varying vec3			ts_viewDir_v;
varying vec3			ts_lightDir_v;

vec3			normal_vs		= normal_vs_v;
vec3			tangent_vs		= tangent_vs_v;
								  
vec3			ts_viewDir		= ts_viewDir_v;
vec3			ts_lightDir		= ts_lightDir_v;

//--------------------------------------------------------------------------------------
// Includes and Defines
//--------------------------------------------------------------------------------------


#define ECCENTRICITY	0.2
#define COMP_ECC 		(ECCENTRICITY * ECCENTRICITY - 1.0)
#define ROLLOFF			0.3

#define REF_INDEX 1.33 	//1.3
#define ROUGHNESS 0.13 	//0.2


//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------


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
		vec3 wHalfAngleDir = normalize(light_vector + -view_vector);

		float coseh = dot(wHalfAngleDir, -view_vector);
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


float getRoughness(float cosa, float m)
{
	return (1.0 / (pow(m, 2.0) * pow(cosa, 4.0))) * exp(-((1.0 / pow(cosa, 2.0) - 1.0) / pow(m, 2.0)));
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


float getModifiedCookTorranceSpecularity(vec3 light_vector, vec3 view_vector, vec3 normal, float refractionindex, float roughness)
{
	float cosln = dot(light_vector, normal);
	float specularity = 0.0;
	if( cosln > 0.0 ){
		vec3 wHalfAngleDir = normalize(light_vector + view_vector);
		float fresnel = getFresnelTerm(refractionindex, dot(view_vector, wHalfAngleDir));
		float rough = getRoughness(dot(normal, wHalfAngleDir), roughness);


		specularity = (rough / dot(normal, light_vector)) *
			max(0.0, (getShadowingAndMaskingTerm(normal, wHalfAngleDir, view_vector, light_vector) / dot(normal, view_vector)) * fresnel / 3.14);
		specularity = clamp(specularity, 0.0, 1.0);
	}
	return specularity;
}





void colorize(out vec4 outColor, in vec3 normal, in vec3 tangent, in vec3 bitangent){
	// Texture lookups.
	vec4 decal_color;
	vec4 normal_color;
	vec4 translucency_color;
	vec4 halflife2_color;
	vec4 cpvcolor		= vec4(1.0);
	vec4 other_cpvcolor	= vec4(1.0);
	ts_lightDir = normalize(ts_lightDir);
	if (gl_FrontFacing)
	{
		decal_color			= pow(	texture2D(frontDecalMap			, gl_TexCoord[0].xy ) , vec4(2.2, 2.2, 2.2, 2.2));
		normal_color		=		texture2D(frontNormalMap		, gl_TexCoord[0].xy );
		translucency_color	= pow(	texture2D(frontTranslucencyMap	, gl_TexCoord[0].xy ) , vec4(2.2, 2.2, 2.2, 2.2));
		halflife2_color		=		texture2D(frontHalfLife2Map		, gl_TexCoord[0].xy );
		
		//cpvcolor = IN.color1;
		//other_cpvcolor = IN.color2;
	}
	else
	{
		decal_color			= pow(	texture2D(backDecalMap			, gl_TexCoord[0].xy ) , vec4(2.2, 2.2, 2.2, 2.2));
		normal_color		=		texture2D(backNormalMap			, gl_TexCoord[0].xy );
		translucency_color	= pow(	texture2D(backTranslucencyMap	, gl_TexCoord[0].xy ) , vec4(2.2, 2.2, 2.2, 2.2));
		halflife2_color		=		texture2D(backHalfLife2Map		, gl_TexCoord[0].xy );

		//cpvcolor = IN.color2;
		//other_cpvcolor = IN.color1;
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

	float translucency;
	float specularity = 0.0;
	float diffuse_term;
	
	// Calculate tangent space normal.
	vec3 ts_normal = normalize(normal_color.xyz * 2.0 - 1.0);

	// Calculate translucency intensity.
	/*translucency =	dot(ts_lightDir, vec3(-SQRT6, -SQRT2, -SQRT3)) * halflife2_color.x +
					dot(ts_lightDir, vec3(-SQRT6,  SQRT2, -SQRT3)) * halflife2_color.y +
					dot(ts_lightDir, vec3( SQRT23,   0.0, -SQRT3)) * halflife2_color.z;
	translucency = max(translucency, 0.0);
	*/
	if (gl_FrontFacing)
	{
		// Calculate translucency intensity.
		
		translucency =	dot(ts_lightDir, vec3(-SQRT6, -SQRT2, SQRT3)) * halflife2_color.x +
					    dot(ts_lightDir, vec3(-SQRT6,  SQRT2, SQRT3)) * halflife2_color.y +
						dot(ts_lightDir, vec3( SQRT23,   0.0, SQRT3)) * halflife2_color.z;
		translucency = max(translucency, 0.0);
		
		// Calculate specularity.
		specularity = getModifiedCookTorranceSpecularity(-ts_lightDir, ts_viewDir, -ts_normal, REF_INDEX, ROUGHNESS);
		//specularity = getPhongSpecularity(IN.direction_to_light, IN.view_vector, ts_normal);
	}
	else
	{
		// Calculate translucency intensity.
		translucency =	dot(ts_lightDir, vec3(-SQRT6, -SQRT2,  SQRT3)) * halflife2_color.x +
						dot(ts_lightDir, vec3(-SQRT6,  SQRT2,  SQRT3)) * halflife2_color.y +
						dot(ts_lightDir, vec3( SQRT23,   0.0,  SQRT3)) * halflife2_color.z;
		translucency = max(translucency, 0.0);

		// Calculate specularity.
		specularity = 0.0;//getModifiedCookTorranceSpecularity(IN.direction_to_light, -IN.view_vector, ts_normal, RefractionIndex, Roughness);
		//specularity = getPhongSpecularity(IN.direction_to_light, -IN.view_vector, ts_normal);
		
		ts_normal.z = -ts_normal.z;
	}

	// Calculate diffuse lighting.
	diffuse_term = max(0.0, dot(ts_lightDir, ts_normal));
	diffuse_term = min(diffuse_term, shadow_intensity);

	// --- End Lighting ---
	
	
	// --- Combine terms for final output color ---
	
	// Actual diffuse light intensity depends on falloff map.

	// Calculate lighting terms.
	///specularity = dot(ts_lightDir, ts_normal);
	
	vec3 translucency_in_light = translucency * other_cpvcolor.rgb * gl_LightSource[0].diffuse.rgb ;

	vec3 final_translucency = translucency_color.rgb * translucency_in_light;// * (shadow_intensity * ReduceTranslucencyInShadow)* MultiplyTranslucency;
	
	vec4 final_ambient = decal_color * cpvcolor * gl_LightSource[0].ambient  * MultiplyAmbient;
	vec4 final_diffuse = decal_color * diffuse_term * gl_FrontLightProduct[0].diffuse * MultiplyDiffuse;
	//vec4 final_diffuse = decal_color * diffuse_term *  MultiplyDiffuse;
	
	
	vec4 final_specular = specularity * shadow_intensity * gl_FrontLightProduct[0].diffuse * MultiplySpecular;
	
	outColor.rgb = final_ambient.rgb + final_diffuse.rgb + final_specular.rgb + final_translucency; //
	
	outColor.a = decal_color.a;
}


void main()
{	
	normal_vs = normalize(normal_vs);
	tangent_vs = normalize(tangent_vs);


	vec3 bitangent = cross(normal_vs, tangent_vs);

	vec4 color;
	colorize(color, normal_vs, tangent_vs, bitangent);
	gl_FragData[0] = color;
	gl_FragData[1] = color * vec4(0.5, 0.5, 0.5, 1.0);


}
