/* sphere.cpp
Example class to create a generic sphere object
Resolution can be controlled by settign nuber of latitudes and longitudes
Referenced Iain Martin October 2018
modified by Aleksejs Loginovs - Feb 2018
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
	GLuint i;
	/* Calculate the number of vertices required in sphere */
	num_verts = 2 + ((numlats - 2) * (numlongs+1));
	this->numlats = numlats;
	this->numlongs = numlongs;

	// Create the arrays to create the sphere vertex attributes
	verts = new glm::vec3[num_verts];
	pTexCoords = new GLfloat[num_verts * 2];
	GLfloat* pColours = new GLfloat[num_verts * 4];
	makeUnitSphere(verts, pTexCoords);

	/* Define colours as the x,y,z components of the sphere vertices */
	for (i = 0; i < num_verts; i++)
	{
		pColours[i * 4] = verts[i].x;
		pColours[i * 4 + 1] = verts[i].y;
		pColours[i * 4 + 2] = verts[i].z;
		pColours[i * 4 + 3] = 1.f;
	}

	/* Generate the vertex buffer object */
	glGenBuffers(1, &sphereBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, sphereBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* num_verts, verts, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	normals = new glm::vec3[num_verts];
	for (GLuint i = 0; i < num_verts; i++)
	{
		normals[i] = glm::normalize(verts[i]);
	}
	/* Store the normals in a buffer object */
	glGenBuffers(1, &sphereNormals);
	glBindBuffer(GL_ARRAY_BUFFER, sphereNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* num_verts, normals, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	/* Store the colours in a buffer object */
	glGenBuffers(1, &sphereColours);
	glBindBuffer(GL_ARRAY_BUFFER, sphereColours);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* num_verts * 4, pColours, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Store the texture coords in a buffer object */
	/* Create the texture coordinate  buffer for the cube */
	if (tex_enabled)
	{
		glGenBuffers(1, &tex_coords_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, tex_coords_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* num_verts * 2, pTexCoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/* Calculate the number of indices in our index array and allocate memory for it */
	num_indices = (numlongs+2)*2/*poles*/ + 2*(numlongs+1)+2*(numlats-4)*(numlongs+1);
	indices = new GLuint[num_indices];

	// fill "indices" to define triangle strips
	GLuint index = 0;		// Current index

	// Define indices for the first triangle fan for south pole
	for (int i = 0; i <= numlongs+1; i++)
	{
		indices[index++] = i;
	}

	// Define latitude indices
	for (i = 0; i < numlats - 3; i++)//correct
	{
		int row_start = i * (numlongs+1) + 1;
		for (int j = 0; j < numlongs+1; j++)
		{
			indices[index++] = row_start + j;
			indices[index++] = row_start + j + (numlongs+1);
		}
		//indices[index++] = row_start;
		//indices[index++] = row_start + numlongs;
	}

	// Define indices for the last triangle fan for the north pole region
	for (i = num_verts - 1; i >= (num_verts - (numlongs+2)); i--)
	{//start from final vert (center of triangle fan for last pole) and add indexes for lower
		indices[index++] = i;
	}
	////indices[index++] = num_verts - 2;
	//for (int i = 0; i < num_indices; i++)
	//	std::cout << "i: " << i << " " << indices[i] << std::endl;

	// Generate a buffer for the indices
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLuint), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete pColours;

	//need to finish the draw function, vertices seem to be generating correctly
	//indices seem to be fine as well (numbers of verts and indices look fine)
	//problem scaling resolution up? something to do with corrupted heap
	//WRITES MORE VERTS THAN SHOULD (overflows vertex array)
}


void Sphere::draw(int drawmode)
{
	/* Bind vertex buffer */
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

	//fill/not fill polygons
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

		/* Draw the south pole regions as a triangle  */
		glDrawElements(GL_TRIANGLE_FAN, numlongs + 2, GL_UNSIGNED_INT, (GLvoid*)(0));

		/* Calculate offsets into the indexed array. Note that we multiply offsets by 4
		because it is a memory offset the indices are type GLuint which is 4-bytes */
		GLuint lat_offset_jump = ((numlongs+1) * 2);
		GLuint lat_offset_start = numlongs + 2;
		GLuint lat_offset_current = lat_offset_start * 4;

		/* Draw the triangle strips of latitudes */
		for (GLuint i = 0; i < numlats - 3; i++)
		{
			glDrawElements(GL_TRIANGLE_STRIP, (numlongs+1) * 2, GL_UNSIGNED_INT, (GLvoid*)(lat_offset_current));
			lat_offset_current += (lat_offset_jump * 4);
		}
		/* Draw the north pole as a triangle fan */
		glDrawElements(GL_TRIANGLE_FAN, numlongs + 2, GL_UNSIGNED_INT, (GLvoid*)((num_indices*4)-(numlongs+2)*4));

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

	//IF RESOLUTION < 20, for some reason writes more latitudes than required
	//make south pole
	verts[0] = { 0.f, -1.f , 0.f };

	texcoords[0] = 0.5f;
	texcoords[1] = 0;

	//make latitude lines
	GLfloat latstep = 180.f / ((float)numlats-1);
	GLfloat longstep = 360.f / (numlongs-1);

	int cur_vert = 1;
	int test = 0;
	for (GLfloat lat = -90.f+latstep; lat <= 90.f-latstep+.01f; lat += latstep)//+1.f is to avoid floating point errors to make sure that if value is 0.0001 larger it still treats it as equal
	{//defines it bottom to top 
		//std::cout << "lat " << lat << std::endl;
		int row_start_vert = cur_vert;
		GLfloat lat_r = glm::radians(lat);

		//moved here for efficiency
		float v = (lat + 90.f) / 180.f;
		int test2 = 0;

		for (GLfloat lon = -180; lon <= 180.01f; lon += longstep)//181.f here to avoid floating point errors (when CPU rounds it incorrectly) same as above
		{
			test2++;
			//std::cout << "lon " << lon << std::endl;
			GLfloat lon_r = glm::radians(lon);
			verts[cur_vert] = { glm::cos(lon_r) * glm::cos(lat_r), glm::sin(lat_r), glm::sin(lon_r) * glm::cos(lat_r)};
			//Define the texture coordinates as normalised lat/long values
			float u = (lon + 180.f) / 360.f;	
			texcoords[cur_vert * 2] = u;
			texcoords[cur_vert * 2 + 1] = v;

			cur_vert++;
		}

		verts[cur_vert] = verts[row_start_vert];
		texcoords[cur_vert * 2] = 1.0f;
		texcoords[cur_vert * 2+1] = v;
		cur_vert++;

		//test2++;
		//std::cout << "test2 " << test2 << std::endl;
		//std::cout << test++ << " of " << numlats - 1 << std::endl;

	}
 	//north pole
	verts[cur_vert] = { 0.f, 1.f, 0.f };

	texcoords[cur_vert * 2] = 0.5f;
	texcoords[cur_vert * 2 + 1] = 1.f;

	std::cout << "vertices generated for sphere: " << cur_vert << " of " << num_verts << std::endl;
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