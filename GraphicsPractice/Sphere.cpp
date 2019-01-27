/* sphere.cpp
Example class to create a generic sphere object
Resolution can be controlled by settign nuber of latitudes and longitudes
Iain Martin October 2018
modified by Aleksejs Loginovs - October 2018
*/

#include "Sphere.h"

/* I don't like using namespaces in header files but have less issues with them in
seperate cpp files */
using namespace std;

/* Define the vertex attributes for vertex positions and normals.
Make these match your application and vertex shader
You might also want to add colours and texture coordinates */
Sphere::Sphere(Shader shader, GLuint textureID)
{
	this->shader = shader;
	attribute_v_coord = 0;
	attribute_v_colours = 1;
	attribute_v_normal = 2;
	attribute_v_tex_coord = 3;
	num_verts = 0;		// We set this when we know the numlats and numlongs values in makeSphere
	this->drawmode = 0;
	
	if (textureID != NULL)
	{
		this->texture_id = textureID;
		tex_enabled = true;
	}
	else
		tex_enabled = false;
}

Sphere::Sphere()
{
	attribute_v_coord = 0;
	attribute_v_colours = 1;
	attribute_v_normal = 2;
	attribute_v_tex_coord = 3;
	num_verts = 0;		// We set this when we know the numlats and numlongs values in makeSphere
	this->drawmode = 0;
}

Sphere::~Sphere()
{
}


/* Make a sphere from two triangle fans (one at each pole) and triangle strips along latitudes */
/* This version uses indexed vertex buffers for both the fans at the poles and the latitude strips */
void Sphere::makeSphere(GLuint numlats, GLuint numlongs)
{
	GLuint i, j;
	/* Calculate the number of vertices required in sphere */
	GLuint numvertices = 2 + ((numlats - 1) * (numlongs));

	// Store the number of sphere vertices in an attribute because we need it later when drawing it
	num_verts = numvertices;
	this->numlats = numlats;
	this->numlongs = numlongs;

	// Create the arrays to create the sphere vertex attributes
	verts = new glm::vec3[numvertices];
	pTexCoords = new GLfloat[numvertices * 2];
	GLfloat* pColours = new GLfloat[numvertices * 4];
	makeUnitSphere(verts, pTexCoords);

	///* Define colours as the x,y,z components of the sphere vertices */
	//for (i = 0; i < numvertices; i++)
	//{
	//	pColours[i * 4] = verts[i * 3];
	//	pColours[i * 4 + 1] = verts[i * 3 + 1];
	//	pColours[i * 4 + 2] = verts[i * 3 + 2];
	//	pColours[i * 4 + 3] = 1.f;
	//}

	/* Generate the vertex buffer object */
	glGenBuffers(1, &sphereBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, sphereBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* numvertices, verts, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	normals = new glm::vec3[numvertices];
	for (GLuint i = 0; i < numvertices; i++)
	{
		normals[i] = glm::normalize(verts[i]);
	}
	/* Store the normals in a buffer object */
	glGenBuffers(1, &sphereNormals);
	glBindBuffer(GL_ARRAY_BUFFER, sphereNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* numvertices, normals, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	/* Store the colours in a buffer object */
	glGenBuffers(1, &sphereColours);
	glBindBuffer(GL_ARRAY_BUFFER, sphereColours);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* numvertices * 4, pColours, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Store the texture coords in a buffer object */
	/* Create the texture coordinate  buffer for the cube */
	if (tex_enabled)
	{
		glGenBuffers(1, &tex_coords_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, tex_coords_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* numvertices * 2, pTexCoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/* Calculate the number of indices in our index array and allocate memory for it */
	num_indices = ((numlongs * 2)) * (numlats - 2) + ((numlongs + 1) * 2);
	indices = new GLuint[num_indices];

	// fill "indices" to define triangle strips
	GLuint index = 0;		// Current index

	// Define indices for the first triangle fan for south pole
	for (int i = 0; i < numlongs+1; i++)
	{
		indices[index++] = i;
	}

	// Define latitude indices
	for (i = 0; i < numlats - 2; i++)
	{
		int row_start = i * numlongs + 1;
		for (int j = 0; j < numlongs; j++)
		{
			indices[index++] = row_start + j;
			indices[index++] = row_start + j + numlongs;
		}
	}

	// Define indices for the last triangle fan for the north pole region
	for (i = numvertices - 1; i > (numvertices - numlongs - 2); i--)
	{
		indices[index++] = i;
	}

	// Generate a buffer for the indices
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLuint), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete pColours;
}


void Sphere::draw(int drawmode)
{
	/* Draw the vertices as GL_POINTS */
	glBindBuffer(GL_ARRAY_BUFFER, sphereBufferObject);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribute_v_coord);

	/* Bind the sphere normals */
	glEnableVertexAttribArray(attribute_v_normal);
	glBindBuffer(GL_ARRAY_BUFFER, sphereNormals);
	glVertexAttribPointer(attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	/* Bind the sphere colours */
	glBindBuffer(GL_ARRAY_BUFFER, sphereColours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribute_v_colours);

	/* Bind the sphere texture coordinates */
	if (tex_enabled)
	{
		glBindBuffer(GL_ARRAY_BUFFER, tex_coords_buffer);
		glVertexAttribPointer(attribute_v_tex_coord, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(attribute_v_tex_coord);
		glBindTexture(GL_TEXTURE_2D, texture_id);
	}

	// Define triangle winding as counter-clockwise
	glFrontFace(GL_CCW);

	glPointSize(3.f);

	// Enable this line to show model in wireframe
	if (drawmode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (drawmode == 2)
	{
		glDrawArrays(GL_POINTS, 0, num_verts);
	}
	else
	{
		/* Bind the indexed vertex buffer */
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		/* Draw the north pole regions as a triangle  */
		glDrawElements(GL_TRIANGLE_FAN, numlongs + 1, GL_UNSIGNED_INT, (GLvoid*)(0));

		/* Calculate offsets into the indexed array. Note that we multiply offsets by 4
		because it is a memory offset the indices are type GLuint which is 4-bytes */
		GLuint lat_offset_jump = (numlongs * 2);
		GLuint lat_offset_start = numlongs + 1;
		GLuint lat_offset_current = lat_offset_start * 4;

		/* Draw the triangle strips of latitudes */
		for (GLuint i = 0; i < numlats - 2; i++)
		{
			glDrawElements(GL_TRIANGLE_STRIP, numlongs * 2, GL_UNSIGNED_INT, (GLvoid*)(lat_offset_current));
			lat_offset_current += (lat_offset_jump * 4);
		}
		/* Draw the south pole as a triangle fan */
		glDrawElements(GL_TRIANGLE_FAN, numlongs + 1, GL_UNSIGNED_INT, (GLvoid*)(lat_offset_current));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	if (tex_enabled)
		glBindTexture(GL_TEXTURE_2D, 0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	this->model_matrix = glm::mat4(1.f);

}

/* Define the vertex positions for a sphere. The array of vertices must have previosuly
been created.
*/
void Sphere::makeUnitSphere(glm::vec3* verts, GLfloat* texcoords)
{
	//make south pole
	verts[0] = { 0.f, -1.f,0.f };

	texcoords[0] = 0.5f;
	texcoords[1] = 0;

	//make latitude lines
	GLfloat latstep = 180.f / (float)numlats;
	GLfloat longstep = 360.f / (numlongs-1);

	int cur_vert = 1;
	for (GLfloat lat = -90 ; lat < 90 - latstep; lat += latstep)//defines it bottom to top
	{
		GLfloat lat_r = glm::radians(lat);
		//+longstep/10.f makes sure that last latitude of verts is drawn
		for (float lon = -180; lon <= 180.f + longstep/10.f; lon += longstep)
		{
			GLfloat lon_r = glm::radians(lon);
			verts[cur_vert] = { glm::cos(lon_r) * glm::cos(lat_r), glm::sin(lat_r), glm::sin(lon_r) * glm::cos(lat_r)};


			//Define the texture coordinates as normalised lat/long values
			float u = (lon + 180.f) / 360.f;
			float v = (lat + 90.f) / 180.f;

			texcoords[cur_vert * 2] = u;
			texcoords[cur_vert * 2 + 1] = v;

			cur_vert++;
		}
	}
	//north pole
	verts[cur_vert] = { 0.f, 1.f, 0.f };

	texcoords[cur_vert * 2] = 0.5f;
	texcoords[cur_vert * 2 + 1] = 1.f;

}

void Sphere::reload_in_memory()
{
	/* Generate the vertex buffer object */
	glGenBuffers(1, &sphereBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, sphereBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* num_verts, verts, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Store the normals in a buffer object */
	glGenBuffers(1, &sphereNormals);
	glBindBuffer(GL_ARRAY_BUFFER, sphereNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* num_verts, normals, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* Draws the sphere form the previously defined vertex and index buffers */
void Sphere::drawSphere(int drawmode)
{

	shader.set_model_view_matrix(view_matrix*model_matrix);
	normals_shader.set_model_view_matrix(view_matrix*model_matrix);

	glUseProgram(shader.get_program_id());
	Sphere::draw(drawmode);
	glUseProgram(0);

	if (draw_normals)
	{
		glUseProgram(normals_shader.get_program_id());
		Sphere::draw(2);
		glUseProgram(0);
	}
	
}


/* IAINS CODE
void Sphere::makeUnitSphere(GLfloat *verts, GLfloat* pTexCoords)
{
	GLfloat DEG_TO_RADIANS = 3.141592f / 180.f;
	GLuint vnum = 0;
	GLfloat x, y, z, lat_radians, lon_radians;

	//Define north pole
	verts[0] = 0; verts[1] = 0; verts[2] = 1.f;
	pTexCoords[0] = 0.5;	pTexCoords[1] = 1.0;
	vnum++;

	GLfloat latstep = 180.f / numlats;
	GLfloat longstep = 360.f / (numlongs - 1);

	//Define vertices along latitude lines
	for (GLfloat lat = 90.f - latstep; lat > -90.f; lat -= latstep)
	{
		lat_radians = lat * DEG_TO_RADIANS;

		// This includes a quick hack to avoid a precion error.
		// lon should be limited to a max of 180.f but in practice this misses
		// out the dateline veretx duplication in some onstances (values on numlongs) so
		// I've increased  lon max to 180.f + longstep / 10.f to ensure that the last
		// set of vertices gets drawn.
		for (GLfloat lon = -180.f; lon <= (180.f + longstep / 10.f); lon += longstep)
		{
			lon_radians = lon * DEG_TO_RADIANS;

			x = cos(lat_radians) * cos(lon_radians);
			y = cos(lat_radians) * sin(lon_radians);
			z = sin(lat_radians);

			//Define the vertex
			verts[vnum * 3] = x; verts[vnum * 3 + 1] = y; verts[vnum * 3 + 2] = z;

			//Define the texture coordinates as normalised lat/long values
			float u = (lon + 180.f) / 360.f;
			float v = (lat + 90.f) / 180.f;

			pTexCoords[vnum * 2] = u;
			pTexCoords[vnum * 2 + 1] = v;

			vnum++;
		}
	}
	//Define south pole
	verts[vnum * 3] = 0; verts[vnum * 3 + 1] = 0; verts[vnum * 3 + 2] = -1.f;
	pTexCoords[vnum * 2] = 0.5; pTexCoords[vnum * 2 + 1] = 0.f;
}
*/