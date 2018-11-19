/* by Aleksejs Loginovs - October 2018 */

#pragma once
#include "Drawable.h"
#include "Constants.h"

class Cube : public Drawable
{
private:
	const int num_verts = NUM_CUBE_VERTS;
	const int num_indices = NUM_CUBE_INDICES;

	/* Define vertices for a cube in 12 triangles */
	glm::vec3 verts[NUM_CUBE_VERTS] =
	{
		// front
		glm::vec3(-1.0, -1.0,  1.0),
		glm::vec3(1.0, -1.0,  1.0),
		glm::vec3(1.0,  1.0,  1.0),
		glm::vec3(-1.0,  1.0,  1.0),
		// back
		glm::vec3(-1.0, -1.0, -1.0),
		glm::vec3(1.0, -1.0, -1.0),
		glm::vec3(1.0,  1.0, -1.0),
		glm::vec3(-1.0,  1.0, -1.0)
	};

	GLuint indices[NUM_CUBE_INDICES] =
	{
		0,1,2,
		2,3,0,
		
		1,5,6,
		6,2,1,
		
		7,6,5,
		5,4,7,
		
		4,0,3,
		3,7,4,
		
		4,5,1,
		1,0,4,

		3,2,6,
		6,7,3
	};

	glm::vec4 colours[NUM_CUBE_VERTS] =
	{
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),

		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
	};

	/* Manually specified normals for our cube (3 values per coordinate) */
	glm::vec3 normals[NUM_CUBE_VERTS] =
	{
		glm::vec3(0, 0, -1.f),
		glm::vec3(0, 0, -1.f),
		glm::vec3(0, 0, -1.f),
		glm::vec3(0, 0, -1.f),
		glm::vec3(0, 0, -1.f),
		glm::vec3(0, 0, -1.f),
		glm::vec3(1.f, 0, 0),
		glm::vec3(1.f, 0, 0)
	};

	glm::vec2 texcoords[NUM_CUBE_INDICES * 2] =
	{
		// ace 0 back
		glm::vec2(.25f, 0.66f),
		glm::vec2(0.25f, 0.33f),
		glm::vec2(.5f, .33f),
		glm::vec2(.5f, .33f),
		glm::vec2(0.5f, .66f),
		glm::vec2(.25f, 0.66f),

		// ace 1 right
		glm::vec2(.5f, 0.34f),
		glm::vec2(0.75f, 0.34f),
		glm::vec2(.5f, .66f),
		glm::vec2(0.75f, 0.34f),
		glm::vec2(0.75f, .66f),
		glm::vec2(.50f, 0.66f),

		// ace 2 front
		glm::vec2(0.75f, .34f),
		glm::vec2(1.f, 0.34f),
		glm::vec2(0.75f, 0.66f),
		glm::vec2(1.f, 0.34f),
		glm::vec2(1.f, .66f),
		glm::vec2(0.75f, 0.66f),

		// ace 3 left
		glm::vec2(.0f, 0.34f),
		glm::vec2(0.25f, 0.34f),
		glm::vec2(.0f, .66f),
		glm::vec2(0.25f, 0.34f),
		glm::vec2(0.25f, .66f),
		glm::vec2(.0f, .66f),

		// ace 4 bottom
		glm::vec2(.25f, 0.f),
		glm::vec2(0.50f, 0.0f),
		glm::vec2(.50f, .33f),
		glm::vec2(.50f, .33f),
		glm::vec2(0.25f, .33f),
		glm::vec2(.25f, 0.f),
		
		// ace 5 top
		glm::vec2(.25f, 0.66f),
		glm::vec2(0.50f, 0.66f),
		glm::vec2(.50f, 1.f),
		glm::vec2(.50f, 1.f),
		glm::vec2(0.25f, 1.f),
		glm::vec2(.25f, 0.66f),
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
	Cube() 
	{
		this->normals_enabled = true;
		this->colours_enabled = true;
	}
	~Cube() {}
};