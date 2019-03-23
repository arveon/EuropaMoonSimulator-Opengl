/* by Aleksejs Loginovs - October 2018 */

#pragma once
#include "Drawable.h"
#include "Constants.h"

class Cube : public Drawable
{
private:
	const int num_verts = NUM_CUBE_VERTS;

	/* Define vertices for a cube in 12 triangles */
	//changed some values to make sure there are no gaps between the sides of the cube (skybox)
	glm::vec3 verts[NUM_CUBE_VERTS] =
	{
		// Face 0 back
		glm::vec3(-1.f, 1.f, -.99f),//1
		glm::vec3(-1.f, -1.f, -.99f),//2
		glm::vec3(1.f, -1.f, -.99f),//3

		glm::vec3(1.f, -1.f, -.99f),//3
		glm::vec3(1.f, 1.f, -.99f),//4
		glm::vec3(-1.f, 1.f, -.99f),//1

		// Face 1 right
		glm::vec3(0.99f, -1.f, -1.f),//3
		glm::vec3(0.99f, -1.f, 1.f),//1
		glm::vec3(0.99f, 1.f, -1.f),//4

		glm::vec3(0.99f, -1.f, 1.f),
		glm::vec3(0.99f, 1.f, 1.f),
		glm::vec3(0.99f, 1.f, -1.f),

		// Face 2 front
		glm::vec3(1.f, -1.f, 0.99f),
		glm::vec3(-1.f, -1.f, 0.99f),
		glm::vec3(1.f, 1.f, 0.99f),

		glm::vec3(-1.f, -1.f, 0.99f),
		glm::vec3(-1.f, 1.f, 0.99f),
		glm::vec3(1.f, 1.f, 0.99f),

		// Face 3 left
		glm::vec3(-.99f, -1.f, 1.f),
		glm::vec3(-.99f, -1.f, -1.f),
		glm::vec3(-.99f, 1.f, 1.f),

		glm::vec3(-.99f, -1.f, -1.f),
		glm::vec3(-.99f, 1.f, -1.f),
		glm::vec3(-.99f, 1.f, 1.f),

		// Face 4 bottom
		glm::vec3(-1.f, -1.f, 1.f),
		glm::vec3(1.f, -1.f, 1.f),
		glm::vec3(1.f, -1.f, -1.f),

		glm::vec3(1.f, -1.f, -1.f),
		glm::vec3(-1.f, -1.f, -1.f),
		glm::vec3(-1.f, -1.f, 1.f),

		// Face 5 top
		glm::vec3(-1.f, 1.f, -1.f),
		glm::vec3(1.f, 1.f, -1.f),
		glm::vec3(1.f, 1.f, 1.f),

		glm::vec3(1.f, 1.f, 1.f),
		glm::vec3(-1.f, 1.f, 1.f),
		glm::vec3(-1.f, 1.f, -1.f)
	};

	glm::vec4 colours[NUM_CUBE_VERTS*4] =
	{
		/* Manually specified colours for our cube */
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),

		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),

		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),

		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),

		glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),

		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)
	};

	/* Manually specified normals for our cube (3 values per coordinate) */
	glm::vec3 normals[NUM_CUBE_VERTS*VALUES_PER_NORMAL] =
	{
		glm::vec3(0, 0, -1.f),
		glm::vec3(0, 0, -1.f),
		glm::vec3(0, 0, -1.f),
		glm::vec3(0, 0, -1.f),
		glm::vec3(0, 0, -1.f),
		glm::vec3(0, 0, -1.f),
		glm::vec3(1.f, 0, 0),
		glm::vec3(1.f, 0, 0),
		glm::vec3(1.f, 0, 0),
		glm::vec3(1.f, 0, 0),
		glm::vec3(1.f, 0, 0),
		glm::vec3(1.f, 0, 0),
		glm::vec3(0, 0, 1.f),
		glm::vec3(0, 0, 1.f),
		glm::vec3(0, 0, 1.f),
		glm::vec3(0, 0, 1.f),
		glm::vec3(0, 0, 1.f),
		glm::vec3(0, 0, 1.f),
		glm::vec3(-1.f, 0, 0),
		glm::vec3(-1.f, 0, 0),
		glm::vec3(-1.f, 0, 0),
		glm::vec3(-1.f, 0, 0),
		glm::vec3(-1.f, 0, 0),
		glm::vec3(-1.f, 0, 0),
		glm::vec3(0, -1.f, 0),
		glm::vec3(0, -1.f, 0),
		glm::vec3(0, -1.f, 0),
		glm::vec3(0, -1.f, 0),
		glm::vec3(0, -1.f, 0),
		glm::vec3(0, 1.f, 0),
		glm::vec3(0, 1.f, 0),
		glm::vec3(0, 1.f, 0),
		glm::vec3(0, 1.f, 0),
		glm::vec3(0, 1.f, 0),
		glm::vec3(0, 1.f, 0)
	};

	glm::vec2 texcoords[NUM_CUBE_VERTS * 2] =
	{
		// ace 0 back
		glm::vec2(.25, 0.66),
		glm::vec2(0.25, 0.33),
		glm::vec2(.5, .33),
		glm::vec2(.5, .33),
		glm::vec2(0.5, .66),
		glm::vec2(.25, 0.66),

		// ace 1 right
		glm::vec2(.5, 0.34),
		glm::vec2(0.75, 0.34),
		glm::vec2(.5, .66),
		glm::vec2(0.75, 0.34),
		glm::vec2(0.75, .66),
		glm::vec2(.50, 0.66),

		// ace 2 front
		glm::vec2(0.75, .34),
		glm::vec2(1., 0.34),
		glm::vec2(0.75, 0.66),
		glm::vec2(1., 0.34),
		glm::vec2(1., .66),
		glm::vec2(0.75, 0.66),

		// ace 3 left
		glm::vec2(.0, 0.34),
		glm::vec2(0.25, 0.34),
		glm::vec2(.0, .66),
		glm::vec2(0.25, 0.34),
		glm::vec2(0.25, .66),
		glm::vec2(.0, .66),

		// ace 4 bottom
		glm::vec2(.25, 0.),
		glm::vec2(0.50, 0.0),
		glm::vec2(.50, .33),
		glm::vec2(.50, .33),
		glm::vec2(0.25, .33),
		glm::vec2(.25, 0.),

		// ace 5 top
		glm::vec2(.25, 0.66),
		glm::vec2(0.50, 0.66),
		glm::vec2(.50, 1.),
		glm::vec2(.50, 1.),
		glm::vec2(0.25, 1.),
		glm::vec2(.25, 0.66)
	};

public:
	void init(Shader shader_program, GLuint texture_id = NULL)
	{
		//only pass texcoords to drawable if texture id is provided, otherwise assume no texture should be used
		if (texture_id != NULL)
			Drawable::init(shader_program, &verts[0], num_verts, &colours[0], nullptr, NULL, &normals[0], &texcoords[0], texture_id);
		else
			Drawable::init(shader_program, &verts[0], num_verts, &colours[0], nullptr, NULL, &normals[0], &texcoords[0]);
	}
	Cube()
	{
		this->normals_enabled = true;
		this->colours_enabled = true;
	}
	~Cube() {}
};