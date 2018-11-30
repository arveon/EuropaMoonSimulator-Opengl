/* by Aleksejs Loginovs - October 2018 */
// Particle fragment shader with fog
#version 420

in vec4 fcolour;
in vec3 fposition;

const float fog_density = 0.025;
const vec4 fog_colour = vec4(0.4,0.4,0.6,1);

in vec2 uv;
uniform sampler2D tex;

out vec4 outputColor;

void main()
{
	vec4 tex_clr = texture(tex, uv);
	outputColor = mix(fcolour,tex_clr,0.5f);
	float thresh = 0.2f;
	if(tex_clr.r < thresh && tex_clr.g < thresh && tex_clr.b < thresh)
	{	//discard all dark fragments
		discard;
	}

	outputColor=tex_clr;

	//add in fog
	float dist = length(fposition.xyz);
	float fog_factor = pow(1.0 / exp(dist*fog_density),2);

	outputColor = mix(fog_colour, outputColor, fog_factor);
}