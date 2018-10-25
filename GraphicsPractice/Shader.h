#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
private:
	GLuint program;

	GLuint model_view_uniform_id;
	GLuint model_uniform_id;
	GLuint view_uniform_id;
	GLuint projection_uniform_id;

	GLuint color_mode_id;
	GLfloat shininess_uniform_id;
	GLuint light_position_uniform_id;
	GLuint is_light_uniform_id;

	GLuint shininess;
public:

	GLuint get_program_id() { return program; };

	void init_shader(GLfloat aspect_ratio);
	void set_view_matrix(glm::mat4);
	void set_model_matrix(glm::mat4);
	void set_model_view_matrix(glm::mat4);
	void set_color_mode(GLuint);
	void set_light_position(glm::vec4);
	void set_shininess(GLuint);

	Shader(GLuint program_id);
	~Shader();
};

