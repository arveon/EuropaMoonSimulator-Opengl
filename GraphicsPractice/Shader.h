/* by Aleksejs Loginovs - October 2018 */
/*
Class represents a shader and has all the methods to manipulate values/uniforms inside it
*/
#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Constants.h"


class Shader
{
private:
	GLuint program;

	GLuint model_view_uniform_id;
	GLuint view_uniform_id;
	GLuint projection_uniform_id;

	GLfloat shininess_uniform_id;
	GLuint light_position_uniform_id;
	GLfloat attenuation_enabled_uniform_id;
	GLfloat texture_enabled_uniform_id;
	GLfloat colour_enabled_uniform_id;
	GLfloat light_enabled_uniform_id;
public:
	GLuint get_program_id() { return program; };

	void init_shader(GLfloat aspect_ratio, int type);
	void set_view_matrix(glm::mat4);
	void set_projection_matrix(glm::mat4);
	void set_model_view_matrix(glm::mat4);
	void set_light_position(glm::vec4);
	void set_shininess(GLfloat);
	void set_attenuation_enabled(GLboolean);
	void set_texture_enabled(GLboolean);
	void set_colour_enabled(GLboolean);
	void set_lighting_enabled(GLboolean);

	Shader(GLuint program_id);
	~Shader();
};

