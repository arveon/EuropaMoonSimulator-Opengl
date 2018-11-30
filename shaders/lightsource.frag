/* by Aleksejs Loginovs - October 2018 */
// Minimal fragment shader with fixed output colour and fog

#version 420

in vec4 fcolour;
in vec3 fposition;

const float fog_density = 0.05;
const vec4 fog_colour = vec4(0.4,0.4,0.6,1);

out vec4 outputColor;

void main()
{
	outputColor = fcolour;

	float dist = length(fposition.xyz);
	float fog_factor = pow(1.0 / exp(dist*fog_density),2);

	outputColor = mix(fog_colour, outputColor, fog_factor);
}