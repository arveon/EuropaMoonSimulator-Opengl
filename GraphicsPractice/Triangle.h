#pragma once
#include <GL/glew.h>
#include "Drawable.h"
#include "Constants.h"


class Triangle : public Drawable
{
private:
	const int num_verts = NUM_TRIANGLE_VERTS;
	float triangle[VALUES_PER_VERT * NUM_TRIANGLE_VERTS] =
	{
		-1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,

		1.0f, -1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,

		.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};

	
public:
	Triangle(const char* shader_path, const char* fragment_shader_path) : Drawable()
	{
		this->init(shader_path, fragment_shader_path, triangle, num_verts);
	}
	~Triangle() {}
};
