/* by Aleksejs Loginovs - October 2018 */
// Minimal fragment shader with fixed output colour

#version 420

in vec4 fcolour;

out vec4 outputColor;

void main()
{
	outputColor = fcolour;
}