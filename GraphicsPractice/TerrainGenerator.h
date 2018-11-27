#pragma once

#include "Drawable.h"
#include "Terrain.h"
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <vector>

#include "FeatureGenerator.h"

class TerrainGenerator
{
private:
	GLuint x_points;
	GLuint z_points;

	GLfloat x_world;
	GLfloat z_world;

	GLfloat altitude_variation_scale;

	GLuint octaves;
	GLfloat perlin_scale;
	GLfloat perlin_freq;

	Drawable* terrain;
	GLuint terrain_texture;

	static GLfloat* calculate_noise(GLuint x_size, GLuint z_size, GLuint octaves, GLuint freq, GLuint scale);

	void scale_heights(int min, int max, glm::vec3* verts, int num_verts);
	void scale_colours(int min, int max, glm::vec4* colours, int num_verts);
	
	void calculate_normals(glm::vec3 * normals, std::vector<GLuint>* elements, glm::vec3* verts);
public:
	Terrain* create_terrain();
	void set_texture(GLuint terrain_texture) { this->terrain_texture = terrain_texture; }

	void apply_terrain_feature(std::vector<glm::vec3> feature, glm::vec3 * terrain, glm::vec2 feature_position, glm::vec2 feature_scale, glm::vec2 feature_resolution, glm::vec2 terrain_resolution, float rotation);
	
	TerrainGenerator();
	~TerrainGenerator();
};

