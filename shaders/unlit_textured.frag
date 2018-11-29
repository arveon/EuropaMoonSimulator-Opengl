/* by Aleksejs Loginovs - October 2018 */
// fragment shader that will only apply texture

#version 420
out vec4 outputColor;

in vec2 ftpos;

uniform sampler2D tex;

void main()
{
	//outputColor = vec4(1-ftpos.x, 1-ftpos.y, 0, 1);
	outputColor = vec4(texture(tex, ftpos).xyz/2, 1);
}