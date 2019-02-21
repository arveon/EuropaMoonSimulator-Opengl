/*	by Aleksejs Loginovs - November 2018
	represents flat terrain drawable object
*/

#include "Terrain.h"


Terrain::Terrain(int num_verts_in_line, glm::vec3 * vertices, int num_verts, glm::vec4 * colours, GLuint * indices, int num_indices, glm::vec3 * normals, glm::vec2 * texcoords, int tex_id)
{
	this->colours_enabled = true;
	this->normals_enabled = true;
	this->init(vertices, num_verts, colours, indices, num_indices, normals, texcoords, tex_id);
	this->set_triangle_strip(num_verts_in_line);
}

void Terrain::draw()
{
	shader_program.set_model_view_matrix(view_matrix * model_matrix);
	normals_shader.set_model_view_matrix(view_matrix * model_matrix);

	glUseProgram(shader_program.get_program_id());
	draw_object(DRAW_TRIANGLE_STRIP_MODE);
	glUseProgram(0);

	if (draw_normals)
	{
		glUseProgram(normals_shader.get_program_id());
		draw_object(DRAW_POINTS_MODE);
		glUseProgram(0);
	}

	//reset the model matrix
	this->model_matrix = glm::mat4(1.0f);
}

Terrain::~Terrain()
{
}
