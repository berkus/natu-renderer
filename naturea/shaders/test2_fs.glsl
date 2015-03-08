#version 120

uniform sampler2D	colorMap;
uniform sampler2D	branch_noise_tex;
uniform sampler2D	leaf_noise_tex;
uniform sampler2D	normalMap;
uniform	sampler2D   dataMap;

uniform float		time;

uniform float		time_offset;
uniform float		wave_amplitude;
uniform float		wave_frequency;
uniform	vec2		movementVectorA;
uniform	vec2		movementVectorB;
uniform float		wave_y_offset;
uniform float		wave_increase_factor;	
uniform vec2		window_size;

uniform vec4		wood_amplitudes;
uniform vec4		wood_frequencies;
uniform float		leaf_amplitude;
uniform float		leaf_frequency;

varying vec3		eyeDir;
varying vec3		normalDir;
varying	float		alpha;

void main()
{	
	
	float sizeFactor = 1.0/max(window_size.x, window_size.y);

	float t = time*10.0*leaf_frequency*sizeFactor+time_offset;
	vec2 movVectorA = movementVectorA;
	vec2 movVectorB = movementVectorB;
	//vec2 movVectorA = vec2(0.0, 1.0);//movementVectorA;
	//vec2 movVectorB = vec2(0.0, 1.0);//movementVectorB;
	vec2 texCoordA = gl_TexCoord[0].st+t*movVectorA;
	vec2 texCoordB = gl_TexCoord[0].st+t*movVectorB; // gl_TexCoord[0].st+t*movVectorB;

	vec2 fpos = gl_TexCoord[0].st;
	
	//fpos = fpos/window_size;
	vec2 oneV2 = vec2(1.0);
	vec2 b0 = texture2D(dataMap, gl_TexCoord[0].st).xy;
	vec2 b1 = texture2D(dataMap, gl_TexCoord[0].st).zw;

		float dist0 = min (1.0, 2.0 * length(fpos - b0)) ; // / branchProjectedLength
		////vec2 b1 = b1_origin * 0.5 + halfV2;
		float dist1 = min (1.0, 5.0 * length(fpos - b1)); // / branchProjectedLength
//
		//b0 =b0*2.0-oneV2;
		//b1 =b1*2.0-oneV2;

		
		float angle;
		float cosA;
		float sinA;
		vec2  difVec;
		mat2 R;
		vec2 rotatedDifVec;
		vec2 newPos = fpos;
		float d = length(b1-vec2(0.5));
		float ti = time*sizeFactor*5.0;
		vec2 si = sizeFactor* 100.0 * wood_amplitudes.xy;
		if ((d>0.01)){
			angle = dist1*(texture2D(branch_noise_tex, (ti * b1 * wood_frequencies.y)).s*2.0 - 1.0)  * si.y;
			//angle = (texture2D(branch_noise_tex, (ti * b1 * wood_frequencies.y)).s*2.0 - 1.0) * si.y;
			
			cosA = cos (angle); 
			sinA = sin (angle);
			difVec = (fpos - b1);
			R = mat2(	cosA	, sinA,
 						-sinA	, cosA );
			rotatedDifVec = R*difVec;
			newPos = b1 + rotatedDifVec;
		}
		//newPos = fpos;
		//angle = (texture2D(branch_noise_tex, (ti * b0 * wood_frequencies.x)).s*2.0-1.0) * si.x;
		angle = dist0*(texture2D(branch_noise_tex, (ti * b0 * wood_frequencies.x)).s*2.0-1.0) * si.x;
		cosA = cos (angle); 
		sinA = sin (angle);
		difVec = (newPos - b0);
		R = mat2(	cosA	, sinA,
 					-sinA	, cosA );
		rotatedDifVec = R*difVec;
		newPos = b0 + rotatedDifVec;

		//texCoordA = (texture2D(displacementMap, texCoordA).st*2.0 - vec2(1.0))*(wave_y_offset + gl_TexCoord[0].t*wave_increase_factor);
		//texCoordB = (texture2D(displacementMap, texCoordB).st*2.0 - vec2(1.0))*(wave_y_offset + gl_TexCoord[0].t*wave_increase_factor);
		texCoordA = (texture2D(leaf_noise_tex, texCoordA).st*2.0 - vec2(1.0));
		texCoordB = (texture2D(leaf_noise_tex, texCoordB).st*2.0 - vec2(1.0));
		
		vec2 texCoord = newPos+(texCoordA+texCoordB)*sizeFactor*leaf_amplitude;// texture2D(displacementMap, ).st;
		vec4 fragmentNormal = texture2D(normalMap, texCoord);
		float branchFlag = fragmentNormal.w + texture2D(normalMap, newPos).w;
		vec4 color;
		if (branchFlag>0.5){
			color = texture2D(colorMap, newPos);
		} else {
			color = texture2D(colorMap, texCoord);
		}
		if (color.a<0.5){discard;}

		color.a = clamp(-2.0 + 4.0*abs(dot(normalize(normalDir), normalize(eyeDir))), 0.0, 1.0);
		
		//color.a =clamp(-0.5 + 2.0*abs(dot(normalize(normalDir), normalize(eyeDir))), 0.0, 1.0);
		//color.a =clamp(abs(dot(normalize(normalDir), normalize(eyeDir))), 0.0, 1.0);
		color.a *= gl_Color.a;
		//color.a = 0.5;
		gl_FragData[0] = color;
		gl_FragData[1] = color * vec4(0.5, 0.5, 0.5, 1.0);

}
