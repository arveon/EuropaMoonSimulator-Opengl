#version 420
/* by Aleksejs Loginovs - October 2018 */
//lit and textured vertex shader with fog

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 tex_coords;


uniform mat4 model_view, view, projection;

uniform vec4 light_position;

out vec3 fposition, fnormal, flightpos;
out vec4 fcolour;
out vec2 ftexCoords;

uniform float shininess = 10;


void main()
{	
	fposition = (model_view * vec4(position,1)).xyz;
	fnormal = transpose(inverse(mat3(model_view)))*normal;
	flightpos = (view * light_position).xyz;
	ftexCoords = tex_coords;
	fcolour=colour;

	gl_Position = projection * model_view * vec4(position,1);
}