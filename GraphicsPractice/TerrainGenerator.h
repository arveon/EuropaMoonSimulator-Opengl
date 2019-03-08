/*	by Aleksejs Loginovs - November 2018 
	Class contains:
	basic terrain generation code adapted from Iain Martin's example code
	sphere terrain generation code written by Aleksejs Loginovs
*/
#pragma once

#include "Drawable.h"
#include "Terrain.h"

#include "Sphere.h"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <vector>

#include "FeatureGenerator.h"
#include "UserPrefs.h"


class TerrainGenerator
{
private:
	GLuint x_points;
	GLuint z_points;

	GLuint num_longs;
	GLuint num_lats;

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
	
	void calculate_normals(glm::vec3 * normals, std::vector<GLuint>* elements, glm::vec3* verts, glm::vec2 resolution);
	void calculate_normals_sphere(glm::vec3 * normals, std::vector<GLuint>* elements, glm::vec3* verts, glm::vec2 resolution);
public:
	Terrain* create_terrain(int xpoints, int zpoints, float x_world, float z_world, GLuint perlin_freq, GLuint scale, GLuint octaves);
	Sphere* create_terrain_on_sphere(Shader shader, int numlats, int numlongs, UserPrefs prefs, GLuint tex = NULL);
	void set_texture(GLuint terrain_texture) { this->terrain_texture = terrain_texture; }

	void apply_terrain_feature(std::vector<glm::vec3> feature, glm::vec3 * terrain, glm::vec2 feature_position, glm::vec2 feature_scale, glm::vec2 feature_resolution, glm::vec2 terrain_resolution, float rotation, bool is_crater = false);
	void apply_terrain_feature_sphere(std::vector<glm::vec3> feature, glm::vec2 scale, glm::vec2 shift, Sphere* sphere, glm::vec2 feature_resolution, int* visited, float rotation_angle = 0);
	void generate_terrain(Sphere* sphere, UserPrefs prefs);
	
	TerrainGenerator();
	~TerrainGenerator();
};

