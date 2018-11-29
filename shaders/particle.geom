#version 400 core
layout(points) in;
layout(triangle_strip, max_vertices=4) out;

uniform mat4 model_view, projection;
float length = 0.5f;
vec3 colour = vec3(1,1,1);
out vec3 fcolour;
out vec2 uv;
out vec3 fposition;

void main()
{
	fcolour = colour;
	
	mat4 mvp = projection * model_view;
	vec4 start = mvp * gl_in[0].gl_Position;
	gl_Position = start;
	fposition = gl_Position.xyz;
	uv=vec2(1,0);
	EmitVertex();

	vec4 a = start;
	a.y += length;
	gl_Position = a;
	fposition = gl_Position.xyz;
	uv=vec2(1,1);
	EmitVertex();

	vec4 b = start;
	b.x -= length;
	gl_Position = b;
	fposition = gl_Position.xyz;
	uv=vec2(0,0);
	EmitVertex();

	vec4 c = start;
	c.x -= length;
	c.y += length;
	gl_Position = c;
	fposition = gl_Position.xyz;
	uv=vec2(0,1);
	EmitVertex();

	EndPrimitive();
}