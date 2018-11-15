/* by Aleksejs Loginovs - October 2018 */

#include "Drawable.h"


void Drawable::init(Shader shader_program, GLfloat* vertices, int num_verts, GLfloat* colours, GLint* indices, int num_indices, GLfloat* normals, GLfloat* texcoords, int tex_id)
{
	//initialises shader, vertices, colors, normals etc if they are used 
	this->shader_program = shader_program;
	verts = vertices;
	this->colours = colours;
	this->normals = normals;
	this->num_verts = num_verts;
	this->num_indices = num_indices;
	this->indices = indices;
	
	if (tex_id != NULL && texcoords)
	{
		tex_enabled = true;
		this->texture_coords = texcoords;
		this->texture_id = tex_id;
	}

	load_into_memory();
}

Drawable::~Drawable()
{

}

//loads all vertex data into GPU memory
void Drawable::load_into_memory()
{
	glGenBuffers(1, &vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * VALUES_PER_VERT * num_verts, verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//load indices into memory
	glGenBuffers(1, &index_buffer_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint)*num_indices, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//load vertex colours into memory
	if (colours_enabled)
	{
		glGenBuffers(1, &colour_buffer_id);
		glBindBuffer(GL_ARRAY_BUFFER, colour_buffer_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * VALUES_PER_VERT * num_verts, colours, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//load vertex normals into memory
	if (normals_enabled)
	{
		glGenBuffers(1, &normal_buffer_id);
		glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * VALUES_PER_NORMAL * num_verts, normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//load vertex texture coords into memory
	if (tex_enabled)
	{
		glGenBuffers(1, &tex_coords_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, tex_coords_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * num_verts, texture_coords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

//draws a drawable object
void Drawable::draw()
{
	shader_program.set_model_view_matrix(view_matrix * model_matrix);

	glUseProgram(shader_program.get_program_id());	

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (colours_enabled)
	{
		glBindBuffer(GL_ARRAY_BUFFER, colour_buffer_id);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, VALUES_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (normals_enabled)
	{
		glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_id);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, VALUES_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (tex_enabled)
	{
		glBindBuffer(GL_ARRAY_BUFFER, tex_coords_buffer);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		glBindTexture(GL_TEXTURE_2D, texture_id);
	}
	
	// Define triangle winding as counter-clockwise
	glFrontFace(GL_CCW);
	glPointSize(3.f);

	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);

	//unbind everything
	if (tex_enabled)
		glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

	//reset the model matrix
	this->model_matrix = glm::mat4(1.0f);
}

void Drawable::set_model_matrix(glm::mat4 model_matrix)
{
	this->model_matrix = model_matrix;
}

void Drawable::set_view_matrix(glm::mat4 view_matrix)
{
	this->view_matrix = view_matrix;
}

void Drawable::translate(glm::vec3 direction)
{
	model_matrix = glm::translate(model_matrix, direction);
	center_position = direction;
}

void Drawable::rotate(float radians, glm::vec3 axis)
{
	model_matrix = glm::rotate(model_matrix, radians, axis);
}

void Drawable::scale(glm::vec3 direction)
{
	model_matrix = glm::scale(model_matrix, direction);
}
