#include "ParticleSystem.h"



void ParticleSystem::create_particles()
{
	//create particles on same height, but with random x and z
	std::vector<glm::vec3> positions;
	for (int i = 0; i < max_particles; i++)
	{
		Particle part = generate_particle();
		//add to list of vertices
		positions.push_back(part.pos);
		particles.push_back(part);
	}


	//create a VBO to store all particle positions
	glGenBuffers(1, &particle_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, particle_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*positions.size(), &positions[0], GL_STREAM_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::draw_particles()
{
	glUseProgram(shader->get_program_id());
	
	glBindBuffer(GL_ARRAY_BUFFER, particle_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if(tex_id != -1)
		glBindTexture(GL_TEXTURE_2D, tex_id);

	glPointSize(7.f);
	glDrawArrays(GL_POINTS, 0, particles.size());

	if(tex_id != -1)
		glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	
}

void ParticleSystem::update_particles(float dt)
{
	shader->set_model_view_matrix(view_matrix*model_matrix);
	//std::cout << particles.at(0).time_lived << std::endl;
	std::vector<glm::vec3> positions;
	for (int i = 0; i < max_particles; i++)
	{
		particles.at(i).time_lived += dt;
		if (particles.at(i).time_lived >= particles.at(i).lifespan)
		{
			particles.erase(particles.begin() + i);
			particles.push_back(generate_particle());
		}
		particles.at(i).pos -= particles.at(i).velocity * dt;
		particles.at(i).pos.x += sin(particles.at(i).time_lived/100)/10;
		particles.at(i).pos.z += cos(particles.at(i).time_lived / 100) / 10;

		positions.push_back(particles.at(i).pos);
	}
	glBindBuffer(GL_ARRAY_BUFFER, particle_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*positions.size(), &positions[0]);

	this->model_matrix = glm::mat4(1.0f);
}

Particle ParticleSystem::generate_particle()
{
	//generate vertex
	glm::vec3 pos = glm::vec3(rand() % max_x, height, rand() % max_z);
	float speed = (rand() % (max_velocity - min_velocity) + min_velocity)/100.f;
	//std::cout << speed << std::endl;
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
