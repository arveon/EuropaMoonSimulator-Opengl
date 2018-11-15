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
	Shader shader = NULL;
public:

	Sphere(Shader shader, GLuint textureID = NULL);
	Sphere() {}
	~Sphere();

	void makeSphere(GLuint numlats, GLuint numlongs);
	void drawSphere(int drawmode);

	// Define vertex buffer object names (e.g as globals)
	GLuint sphereBufferObject;
	GLuint sphereNormals;
	GLuint sphereColours;
	GLuint elementbuffer;

	GLuint attribute_v_coord;
	GLuint attribute_v_normal;
	GLuint attribute_v_colours;
	GLuint attribute_v_tex_coord;

	int numspherevertices;
	unsigned int numlats;
	unsigned int numlongs;
	int drawmode;

private:
	void makeUnitSphere(GLfloat *pVertices, GLfloat* pTexCoords);
};


