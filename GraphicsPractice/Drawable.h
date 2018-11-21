/* by Aleksejs Loginovs - October 2018 */

#pragma once
#include <vector>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderManager.h"
#include "Constants.h"

class Drawable
{
protected:
	GLuint vertex_buffer_id;
	GLuint colour_buffer_id;
	GLuint normal_buffer_id;
	GLuint model_uniform_id;
	GLuint tex_coords_buffer;
	GLuint index_buffer_id;

	GLuint texture_id;

	Shader shader_program = NULL;
	Shader normals_shader = NULL;

	glm::vec3 *verts = nullptr;
	glm::vec4 *colours = nullptr;
	glm::vec3 *normals = nullptr;
	glm::vec2 *texture_coords = nullptr;
	GLuint *indices = nullptr;
	
	int num_verts;
	int num_indices;

	glm::mat4 model_matrix;
	glm::mat4 view_matrix;

	glm::vec3 center_position = glm::vec4(0);

	bool draw_normals;

	void load_into_memory();
	void draw_object(int mode = 1);
public:
	Drawable() {};
	~Drawable();
	virtual void init(glm::vec3* vertices, int num_verts, glm::vec4* colours, GLuint* indices, int num_indices, glm::vec3* normals = nullptr, glm::vec2* texcoords = nullptr, int tex_id = NULL);
	virtual void init(Shader shader_program, glm::vec3* vertices, int num_verts, glm::vec4* colours, GLuint* indices, int num_indices, glm::vec3* normals = nullptr, glm::vec2* texcoords = nullptr, int tex_id = NULL);

	void set_shader(Shader shader_program) { this->shader_program = shader_program; }

	void set_draw_normals(bool value) { this->draw_normals = value; }
	void set_normal_shader(Shader normal_shader) { this->normals_shader = normal_shader; }
	
	bool tex_enabled;
	bool normals_enabled;
	bool colours_enabled;
	
	void draw();
	void set_model_matrix(glm::mat4 model_matrix);
	void set_view_matrix(glm::mat4 view_matrix);

	glm::mat4 get_model_matrix() { return model_matrix; }
	glm::vec3 get_center_position() { return center_position; }

	void translate(glm::vec3 direction);
	void rotate(float radians, glm::vec3 axis);
	void scale(glm::vec3 direction);
};

