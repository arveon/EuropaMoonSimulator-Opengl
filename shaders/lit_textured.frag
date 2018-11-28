/* by Aleksejs Loginovs - October 2018 */
//lit and textured fragment shader

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


vec4 ambient_colour = vec4(0.2, 0.2, 0.2, 1);
vec4 diffuse_colour = vec4(0.3, 0.3, 0.3, 1);
vec4 specular_colour = vec4(1, 1, 1, 1);

float lightsource_strength = 0.1;


vec4 sunlight = vec4(1,1,1,1);
vec3 sun_direction = vec3(-1,30,0);
float sun_strength = 0.3;


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

		//diffuse
		vec4 diffuse = max(dot(fnormal_n, to_light), 0) * diffuse_colour * lightsource_strength;
	
		//specular
		vec3 normalised_vert = normalize(-fposition.xyz);
		vec3 reflection = reflect(-to_light, fnormal_n);
		vec4 specular = pow(max(dot(reflection, normalised_vert), 0), shininess) * specular_colour * lightsource_strength;

		float attenuation = 1;
		if(attenuation_enabled)
		{
			float k1 = 0.01f;
			attenuation = 1.0 / (k1 + k1*distance_to_light + k1*pow(distance_to_light, 2));
		}

		//calculate the sunlight components (directional light)
		sun_direction = normalize(sun_direction);
		float sun_diffuse = dot(fnormal_n, sun_direction);
		diffuse += max(sun_diffuse,0) * sunlight * sun_strength;

		//outputColor = mix(attenuation*(diffuse + specular) + ambient_colour, texcolor, .6);
		outputColor = (attenuation*(diffuse + specular) + ambient_colour)*texcolor;
		//outputColor = diffuse;
	}
	else
		outputColor = vert_colour;
}