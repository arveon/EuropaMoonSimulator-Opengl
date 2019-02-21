/*	by Aleksejs Loginovs - November 2018
	manages snow particles
*/

#include "ParticleSystem.h"

void ParticleSystem::create_particles(int max_x, int max_z, int min_velocity, int max_velocity, int start_height)
{
	this->max_x = max_x;
	this->max_z = max_z;
	this->min_velocity = min_velocity;
	this->max_velocity = max_velocity;
	this->height = start_height;


	//create particles on same height, but with random x and z
	std::vector<glm::vec3> positions;
	for (int i = 0; i < max_particles; i++)
	{
		Particle part = generate_particle();
		//add to list of vertices and list of particles
		positions.push_back(part.pos);
		particles.push_back(part);
	}

	//create a VBO to store all particle positions
	glGenBuffers(1, &particle_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, particle_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*positions.size(), &positions[0], GL_STREAM_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*
Function used to draw all particles
*/
void ParticleSystem::draw_particles()
{
	glUseProgram(shader->get_program_id());
	
	glBindBuffer(GL_ARRAY_BUFFER, particle_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if(tex_id != -1)
		glBindTexture(GL_TEXTURE_2D, tex_id);

	glDrawArrays(GL_POINTS, 0, particles.size());

	if(tex_id != -1)
		glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	
}

/*
Function updates particles positions, removes dead particles and adds new particles if more needed
*/
void ParticleSystem::update_particles(float dt)
{
	shader->set_model_view_matrix(view_matrix*model_matrix);
	
	std::vector<glm::vec3> positions;
	for (int i = 0; i < max_particles; i++)
	{
		particles.at(i).time_lived += dt;
		if (particles.at(i).time_lived >= particles.at(i).lifespan)
		{//if particle should die, remove it and create a new one
			particles.erase(particles.begin() + i);
			particles.push_back(generate_particle());
		}
		//update particle position (falling down and x&z should follow a periodic function)
		particles.at(i).pos -= particles.at(i).velocity * dt;
		particles.at(i).pos.x += sin(particles.at(i).time_lived/100)/10;
		particles.at(i).pos.z += cos(particles.at(i).time_lived / 100) / 10;

		positions.push_back(particles.at(i).pos);
	}
	glBindBuffer(GL_ARRAY_BUFFER, particle_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*positions.size(), &positions[0]);

	this->model_matrix = glm::mat4(1.0f);
}

/*
Function used to generate a single particle with position and velocity in a certain direction
!!!CHANGE VELOCITY (go towards centre) AND LIFESPAN TO ADAPT TO SPHERE!!!
*/
Particle ParticleSystem::generate_particle()
{
	//generate vertex
	glm::vec3 pos = glm::vec3(rand() % max_x, height, rand() % max_z);
	float speed = (rand() % (max_velocity - min_velocity) + min_velocity)/100.f;
	//create an actual particle
	Particle part =
	{
		pos,
		glm::vec3(0, speed, 0),
		0,
		rand() % 600 + 800
	};

	return part;
}

void ParticleSystem::translate(glm::vec3 direction)
{
	model_matrix = glm::translate(model_matrix, direction);
}

void ParticleSystem::rotate(float radians, glm::vec3 axis)
{
	model_matrix = glm::rotate(model_matrix, radians, axis);
}

void ParticleSystem::scale(glm::vec3 direction)
{
	model_matrix = glm::scale(model_matrix, direction);
}

ParticleSystem::~ParticleSystem()
{
}
