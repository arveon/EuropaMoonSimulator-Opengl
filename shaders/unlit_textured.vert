/* by Aleksejs Loginovs - October 2018 */
#version 420

layout(location = 0) in vec4 position;
layout(location = 3) in vec2 tex_pos;
uniform mat4 model_view, projection;

out vec2 ftpos;
out vec4 fcolour;


void main()
{	
	ftpos = tex_pos;
	gl_Position = projection * model_view * position;
}