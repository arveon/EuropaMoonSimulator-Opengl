/* by Aleksejs Loginovs - October 2018 */
#version 420

layout(location = 0) in vec3 position;
out vec4 fcolour;

uniform mat4 model_view, projection;

void main()
{	
	fcolour = vec4(1,1,1,1);
	gl_Position = projection * model_view * vec4(position,1);
}