/* by Aleksejs Loginovs - October 2018 */

#include "Shader.h"

void Shader::init_shader(GLfloat aspect_ratio, int type)
{
	glUseProgram(program);
	
	//allocate modelview matrix
	model_view_uniform_id = glGetUniformLocation(program, "model_view");
	glm::mat4 model = glm::mat4(1.f);
	glUniformMatrix4fv(model_view_uniform_id, 1, GL_FALSE, &model[0][0]);

	//projection
	projection_uniform_id = glGetUniformLocation(program, "projection");
	//set initial matrix value to identity matrix
	glm::mat4 projection = glm::perspective(glm::radians(60.f), aspect_ratio, 0.1f, 100.f);
	glUniformMatrix4fv(projection_uniform_id, 1, GL_FALSE, &projection[0][0]);

	if (type == BASIC_SHADER)
	{
		//allocate shininess uniform and set to 10
		shininess_uniform_id = glGetUniformLocation(program, "shininess");
		GLuint shininess = 10;
		glUniform1ui(shininess_uniform_id, shininess);

		light_position_uniform_id = glGetUniformLocation(program, "light_position");	
		glm::vec4 pos = glm::vec4(0, 0, 0, 1);
		glUniform4fv(light_position_uniform_id, 1, &pos[0]);

		//allocate view matrix
		view_uniform_id = glGetUniformLocation(program, "view");
		glm::mat4 model = glm::mat4(1.f);
		glUniformMatrix4fv(view_uniform_id, 1, GL_FALSE, &model[0][0]);

		texture_enabled_uniform_id = glGetUniformLocation(program, "texture_enabled");
		glUniform1i(texture_enabled_uniform_id, false);

		attenuation_enabled_uniform_id = glGetUniformLocation(program, "attenuation_enabled");
		glUniform1i(attenuation_enabled_uniform_id, true);
	}



	glUseProgram(0);
}

void Shader::set_view_matrix(glm::mat4 view)
{
	glUseProgram(program);
	glUniformMatrix4fv(view_uniform_id, 1, GL_FALSE, &view[0][0]);
	glUseProgram(0);
}

void Shader::set_projection_matrix(glm::mat4 proj)
{
	glUseProgram(program);
	glUniformMatrix4fv(projection_uniform_id, 1, GL_FALSE, &proj[0][0]);
	glUseProgram(0);
}

void Shader::set_model_view_matrix(glm::mat4 model_view)
{
	glUseProgram(program);
	glUniformMatrix4fv(model_view_uniform_id, 1, GL_FALSE, &model_view[0][0]);
	glUseProgram(0);
}

void Shader::set_light_position(glm::vec4 light_pos)
{
	glUseProgram(program);
	glUniform4fv(light_position_uniform_id, 1, &light_pos[0]);
	glUseProgram(0);
}

void Shader::set_shininess(GLfloat shininess)
{
	glUseProgram(program);
	glUniform1f(shininess_uniform_id, shininess);
	glUseProgram(0);
}

void Shader::set_attenuation_enabled(GLboolean enabled)
{
	glUseProgram(program);
	glUniform1i(attenuation_enabled_uniform_id, enabled);
	glUseProgram(0);
}

void Shader::set_texture_enabled(GLboolean enabled)
{
	glUseProgram(program);
	glUniform1i(texture_enabled_uniform_id, enabled);
	glUseProgram(0);
}

Shader::Shader(GLuint program_id)
{
	this->program = program_id;
}

Shader::~Shader()
{
}
