/* by Aleksejs Loginovs - November 2018 */
#version 420

layout(location = 0) in vec3 position;

void main()
{	
	gl_Position = vec4(position,1);
}