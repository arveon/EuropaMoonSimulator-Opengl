#pragma once
#include <vector>
#include <glm\common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gl/glew.h>
#include <iostream>

#include "Shader.h"


struct Particle
{
	glm::vec3 pos;
	glm::vec3 velocity;
	float time_lived;
	float lifespan;
};

class ParticleSystem
{
private:
	int max_particles = 1000;
	int max_x, max_z;
	int height;
	int min_velocity, max_velocity;
	std::vector<Particle> particles;
	float point_size;

	GLuint particle_vbo;
	GLuint tex_id = -1;
	Shader* shader;

	glm::mat4 model_matrix;
	glm::mat4 view_matrix;
public:
	void create_particles();
	void draw_particles();
	void update_particles(float dt);

	void set_model_matrix(glm::mat4 model_matrix) { this->model_matrix = model_matrix; }
	void set_view_matrix(glm::mat4 view_matrix) { this->view_matrix = view_matrix; }

	Particle generate_particle();

	void translate(glm::vec3 direction);
	void rotate(float radians, glm::vec3 axis);
	void scale(glm::vec3 direction);

	void set_shader(Shader* shader) { this->shader = shader; }
	void set_texture(GLuint tex_id) { this->tex_id = tex_id; }

	ParticleSystem() 
	{
		point_size = 10;
		max_x = 90;
		max_z = 90;
		min_velocity = 3;
		max_velocity = 8;
		height = 30;
		this->model_matrix = glm::mat4(1.f);
	}
	~ParticleSystem();
};

