#version 400 core
layout(points) in;
layout(line_strip, max_vertices=2) out;

uniform mat4 model_view, projection;
float length = 1;
vec3 colour = vec3(1,1,0);

in vec3 vnormal[];
out vec3 fcolour;

void main()
{
	fcolour = colour;
	
	mat4 mvp = projection * model_view;
	vec4 start = mvp * gl_in[0].gl_Position;
	gl_Position = start;
	EmitVertex();

	vec4 mvp_normal = normalize(mvp * vec4(vnormal[0],0)) * length;

	vec4 end = start + mvp_normal;
	gl_Position = end;
	EmitVertex();

	EndPrimitive();
}