/* by Aleksejs Loginovs - November 2018 */
#pragma once
#include "Drawable.h"
class Terrain :
	public Drawable
{
private:

public:
	Terrain(int num_verts_in_line, glm::vec3* vertices, int num_verts, glm::vec4* colours, GLuint* indices, int num_indices, glm::vec3* normals = nullptr, glm::vec2* texcoords = nullptr, int tex_id = NULL);
	void draw();
	~Terrain();
};

