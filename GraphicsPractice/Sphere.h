/* sphere.h
Example class to create a generic sphere object
Resolution can be controlled by settign nuber of latitudes and longitudes
Iain Martin November 2018
modified by Aleksejs Loginovs - November 2018
*/

#pragma once

#include <gl/glew.h>
#include <vector>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Drawable.h"

class Sphere : public Drawable
{
private:
	friend class TerrainGenerator;
	friend class ObjectLoader;
public:
	Sphere(Shader shader, GLuint textureID = NULL);
	Sphere();
	~Sphere();

	

	void makeSphere(GLuint numlats, GLuint numlongs);
	void drawSphere(int drawmode);

	//int numspherevertices;
	unsigned int numlats;
	unsigned int numlongs;
	int drawmode;
	void set_shader(Shader shader) { this->shader_program = shader; }
	void set_texture(GLuint texture) { Drawable::texture_id = texture; }

	void produce_indices();
	void reload_in_memory();
private:
	void draw(int drawmode);
	void makeUnitSphere(glm::vec3 *pVertices, glm::vec2* pTexCoords);
	

	void set_normals(glm::vec3* normals) { this->normals = normals; }
	void set_verts(glm::vec3* verts) { this->verts = verts; }
	void set_texcoords(glm::vec2* texcoords) { this->texture_coords = texcoords; }

	void gen_buffers();
};


