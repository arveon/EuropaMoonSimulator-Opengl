// Minimal fragment shader with fixed output colour

#version 420

in vec4 fcolour;

in vec3 fposition, fnormal, flightpos;
in vec4 fdiffuse, fspecular, fambient;

out vec4 outputColor;
uniform float shininess = 10;


uniform bool attenuation_enabled;

void main()
{
	//get light direction and distance to light
	vec3 to_light = flightpos - fposition;
	float distance_to_light = length(to_light);
	to_light = normalize(to_light);

	//diffuse
	vec4 diffuse = max(dot(fnormal, to_light), 0) * fdiffuse;
	
	//specular
	vec3 normalised_vert = normalize(-fposition.xyz);
	vec3 reflection = reflect(-to_light, fnormal);
	vec4 specular = pow(max(dot(reflection, normalised_vert), 0), shininess) * fspecular;

	float attenuation = 1;
	if(attenuation_enabled)
	{
		float k1 = .5;
		attenuation = 1.0 / (k1 + k1*distance_to_light + k1*pow(distance_to_light, 2));
	}

	outputColor = attenuation * (diffuse + specular) + fambient;
	//outputColor = fcolour;
}