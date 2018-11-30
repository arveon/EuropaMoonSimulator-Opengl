#version 400 core
/*	by Aleksejs Loginovs - November 2018 
	adapted from Iain Martin's normals example code
*/

out vec4 colour;
in vec3 fcolour;

void main()
{
	colour = vec4(fcolour, 1);
	colour = vec4(1,0,0,1);
}