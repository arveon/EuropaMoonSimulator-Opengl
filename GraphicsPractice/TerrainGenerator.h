#pragma once

#include "Drawable.h"
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>


class TerrainGenerator
{
private:
	GLuint x_points;
	GLuint z_points;

	GLfloat x_world;
	GLfloat z_world;

	GLuint octaves;
	GLfloat perlin_scale;
	GLfloat perlin_freq;

	Drawable* terrain;

	static GLfloat* calculate_noise(GLuint x_size, GLuint z_size, GLuint octaves, GLuint freq, GLuint scale);
	
	void calculate_normals(glm::vec3 * normals, std::vector<GLuint> elements, glm::vec3* verts);
public:
	Drawable* create_terrain();
	
	TerrainGenerator();
	~TerrainGenerator();
};

