/* by Aleksejs Loginovs - October 2018 */

#pragma once
#include "Drawable.h"
#include "Constants.h"

class Quad : public Drawable
{
private:
	const int num_verts = NUM_SQR_VERTS;
	const int num_indices = NUM_SQR_INDICES;

	/* Define vertices for a cube in 12 triangles */
	glm::vec3 verts[NUM_SQR_VERTS] =
	{
		// front
		glm::vec3(-1.0, -1.0,  0),
		glm::vec3(1.0, -1.0,  0),
		glm::vec3(1.0,  1.0,  0),
		glm::vec3(-1.0,  1.0,  0),
	};

	GLuint indices[NUM_SQR_INDICES] =
	{
		0,3,2,
		2,1,0
	};

	glm::vec4 colours[NUM_SQR_VERTS] =
	{
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	};

	/* Manually specified normals for our cube (3 values per coordinate) */
	glm::vec3 normals[NUM_SQR_VERTS] =
	{
		glm::vec3(0, 0, 1.f),
		glm::vec3(0, 0, 1.f),
		glm::vec3(0, 0, 1.f),
		glm::vec3(0, 0, 1.f)
	};

	glm::vec2 texcoords[NUM_CUBE_INDICES * 2] =
	{
		glm::vec2(0,0),
		glm::vec2(1,0),
		glm::vec2(1,1),
		glm::vec2(0,1)
	};

public:
	void init(Shader shader_program, GLuint texture_id = NULL)
	{
		//only pass texcoords to drawable if texture id is provided, otherwise assume no texture should be used
		if (texture_id != NULL)
			Drawable::init(shader_program, verts, num_verts, colours, indices, num_indices, normals, texcoords, texture_id);
		else
			Drawable::init(shader_program, verts, num_verts, colours, indices, num_indices, normals, texcoords);
	}
	Quad()
	{
		this->normals_enabled = false;
		this->colours_enabled = false;
	}
	~Quad() {}
};