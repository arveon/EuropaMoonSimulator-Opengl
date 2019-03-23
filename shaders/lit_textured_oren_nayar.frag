/* by Aleksejs Loginovs - October 2018 */
//lit and textured fragment shader with fog

#version 420

in vec3 fposition, fnormal, flightpos;
in vec4 fcolour;
in vec2 ftexCoords;

out vec4 outputColor;

uniform float shininess;
uniform sampler2D tex;
uniform bool attenuation_enabled;
uniform bool texture_enabled;
uniform bool colour_enabled;
uniform bool light_enabled;


vec4 ambient_colour = vec4(0.1, 0.1, 0.1, 1);
vec4 diffuse_colour = vec4(0.1, 0.1, 0.1, 1);

const float fog_density = 0.02;
const vec4 fog_colour = vec4(0,0,0,1);

const float PI = 3.141592653;
const float roughness=0.99;

void main()
{
	//get texture color
	vec4 texcolor = vec4(1.f);
	if(texture_enabled)
		texcolor = texture(tex, ftexCoords);

	vec4 vert_colour = vec4(1.f);
	if(colour_enabled)
		vert_colour = fcolour;

	if(light_enabled)
	{
		//get light direction and distance to light
		vec3 to_light = flightpos - fposition;
		float distance_to_light = length(to_light);
		to_light = normalize(to_light);
		vec3 fnormal_n = normalize(fnormal);
		vec3 view_dir = normalize(-fposition.xyz);
		

		// Calculate these dot products which are used in the Oren Nayar equations below
		float NdotL = dot(fnormal_n, to_light);
		float NdotV = dot(fnormal_n, view_dir); 

		float angleVN = acos(NdotV);
		float angleLN = acos(NdotL);

		// Implement the Oren Nayar equations
		float alpha = max(angleVN, angleLN);
		float beta = min(angleVN, angleLN);
		float gamma = dot(view_dir - fnormal_n * dot(view_dir, fnormal_n), to_light - fnormal_n * dot(to_light, fnormal_n));

		float roughnessSquared = roughness * roughness;
		float roughnessSquared9 = (roughnessSquared / (roughnessSquared + 0.09));

		// calculate C1, C2 and C3
		float C1 = 1.0 - 0.5 * (roughnessSquared / (roughnessSquared + 0.33));
		float C2 = 0.45 * roughnessSquared9;

		if(gamma >= 0.0) {
			C2 *= sin(alpha);
		} else {
			C2 *= (sin(alpha) - pow((2.0 * beta) / PI, 3.0));
		}

		float powValue = (4.0 * alpha * beta) / (PI * PI);
		float C3  = 0.125 * roughnessSquared9 * powValue * powValue;

		// now calculate both main parts of the formula
		float A = gamma * C2 * tan(beta);
		float B = (1.0 - abs(gamma)) * C3 * tan((alpha + beta) / 2.0);

		// put it all together
		float L1 = max(0.0, NdotL) * (C1 + A + B);

		// also calculate interreflection
		float twoBetaPi = 2.0 * beta / PI;

		float L2 = 0.17 * max(0.0, NdotL) * (roughnessSquared / (roughnessSquared + 0.13)) * (1.0 - gamma * twoBetaPi * twoBetaPi);

		vec4 oren_nayar = vec4(diffuse_colour.xyz * (L1 + L2), 1.0);

		//attenuation quite low
		float attenuation = 1;
		if(attenuation_enabled)
		{
			float k1 = 0.005f;
			attenuation = 1.0 / (k1 + k1*distance_to_light + k1*pow(distance_to_light, attenuation));
		}

		outputColor = (attenuation*(oren_nayar))*texcolor;

		//add in exponential fog
		float dist = length(fposition.xyz);
		float fog_factor = 1.0 / exp(dist*fog_density);

		//outputColor = mix(fog_colour, outputColor, fog_factor);
	}
	else
		outputColor = vert_colour;
}