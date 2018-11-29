/* by Aleksejs Loginovs - October 2018 */
#version 420

layout(location = 0) in vec3 position;
out vec4 fcolour;
out vec3 fposition;

uniform mat4 model_view, projection;


void main()
{	
	fposition = (model_view * vec4(position,1)).xyz;
	fcolour = vec4(1,1,1,1);
	gl_Position = projection * model_view * vec4(position,1);
}