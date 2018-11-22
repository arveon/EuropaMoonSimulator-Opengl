#include "TerrainGenerator.h"



GLfloat * TerrainGenerator::calculate_noise(GLuint x_size, GLuint z_size, GLuint octaves, GLuint frequency, GLuint scl)
{
	int num_verts = x_size * z_size;
	GLfloat* noise = new GLfloat[num_verts * octaves];
	for (int i = 0; i < num_verts*octaves; i++)
		noise[i] = 0;

	//the amount noise will be moved per unit
	GLfloat xfactor = 1.f / (x_size - 1);
	GLfloat zfactor = 1.f / (z_size - 1);

	for (GLuint row = 0; row < z_size; row++)
	{
		for (GLuint col = 0; col < x_size; col++)
		{
			//get coords of where noise will be sampled
			GLfloat x = xfactor * col;
			GLfloat z = zfactor * row;

			GLfloat sum = 0;
			GLfloat cur_scale = scl;
			GLfloat cur_freq = frequency;

			//sum for each octave
			for (GLuint oct = 0; oct < octaves; oct++)
			{
				glm::vec2 point(x*cur_freq, z*cur_freq);
				GLfloat val = glm::perlin(point) / cur_scale;
				sum += val;

				GLfloat res = (sum + 1.f) / 2.f;//make sure sum value is always between 0 and 1
				noise[(row*x_size + col)*octaves + oct] = res;

				cur_freq *= 2.f;
				cur_scale *= scl;
			}
		}
	}

	return noise;
}

void TerrainGenerator::scale_heights(int min, int max, glm::vec3* verts, int num_verts)
{
	GLfloat cmin, cmax;
	cmin = cmax = verts[0].y;

	//find min and max in current scale
	for (int i = 0; i < num_verts; i++)
	{
		if (verts[i].y < cmin) cmin = verts[i].y;
		if (verts[i].y > cmax) cmax = verts[i].y;
	}

	GLfloat stretch_factor = (max - min)/(cmax-cmin);//desired range / current range
	GLfloat min_diff = cmin - min;
	for (int i = 0; i < num_verts; i++)
	{
		verts[i].y = (verts[i].y - min_diff) * stretch_factor;
	}
}

void TerrainGenerator::scale_colours(int min, int max, glm::vec4* colours, int num_verts)
{
	GLfloat cmin, cmax;
	cmin = cmax = colours[0].y;

	//find min and max in current scale
	for (int i = 0; i < num_verts; i++)
	{
		if (colours[i].y < cmin) cmin = colours[i].y;
		if (colours[i].y > cmax) cmax = colours[i].y;
	}

	GLfloat stretch_factor = (max - min) / (cmax - cmin);//desired range / current range
	GLfloat min_diff = cmin - min;
	for (int i = 0; i < num_verts; i++)
	{
		colours[i].y = (colours[i].y - min_diff) * stretch_factor;
		colours[i].x = colours[i].z = colours[i].y;
	}
}

void TerrainGenerator::calculate_normals(glm::vec3 * normals, std::vector<GLuint> elements, glm::vec3* verts)
{
	GLuint element_pos = 0;
	glm::vec3 AB, AC, cross;

	//through lines
	for (GLuint x = 0; x < x_points-1; x++)
	{
		//through triangles in strip
		for (GLuint z = 0; z < z_points*2-2; z++)
		{
			GLuint v1 = elements[element_pos];
			GLuint v2 = elements[element_pos+1];
			GLuint v3 = elements[element_pos+2];

			AB = verts[v2] - verts[v1];
			AC = verts[v3] - verts[v1];

			cross = glm::normalize(glm::cross(AC, AB));
			normals[v1] += cross;
			normals[v2] += cross;
			normals[v3] += cross;

			element_pos++;
		}

		element_pos += 2;
	}
	//normalise normals
	for (GLuint v = 0; v < x_points*z_points; v++)
	{
		normals[v] = glm::normalize(normals[v]);
		//normals[v].x = -normals[v].x;
		std::cerr << normals[v].x << " " << normals[v].y << " " << normals[v].z << std::endl;
	}

}

Drawable* TerrainGenerator::create_terrain()
{
	GLuint num_verts = x_points * z_points;

	glm::vec3* verts = new glm::vec3[num_verts];
	glm::vec3* normals = new glm::vec3[num_verts];
	glm::vec4* colours= new glm::vec4[num_verts];

	//calculate noise
	//allocate and set to 0
	GLfloat* noise = TerrainGenerator::calculate_noise(x_points, z_points, octaves, perlin_freq, perlin_scale);

	GLfloat xpos_start = -x_world / 2.f;
	GLfloat zpos_start = -z_world / 2.f;

	GLfloat xpos_step = x_world / GLfloat(x_points);
	GLfloat zpos_step = z_world / GLfloat(z_points);

	//generate vertices
	for (GLuint x = 0; x < x_points; x++)
	{
		GLfloat xpos = x * xpos_step;
		for (GLuint z = 0; z < z_points; z++)
		{
			GLfloat zpos = z * zpos_step;
			GLfloat height = (noise[(x*z_points + z) * octaves]);
			int cur_index = x*x_points + z;
			verts[cur_index] = glm::vec3(xpos, height, zpos);//x
			normals[cur_index] = glm::vec3(0, 1.f, 0);//x
			colours[cur_index] = glm::vec4(height, height, height, 1.0f);
			//std::cerr << height << std::endl;
		}
	}

	std::vector<GLuint> elements;
	//calculate vertex order into element array
	for (GLuint x = 0; x < x_points-1; x++)
	{
		GLuint top = x * z_points;//start of cur row
		GLuint bottom = top + z_points;//start of next row
		for (GLuint z = 0; z < z_points; z++)
		{
			elements.push_back(top + z);
			elements.push_back(bottom + z);
		}
	}

	scale_heights(0, 3, verts, num_verts);
	scale_colours(0, 1, colours, num_verts);
	calculate_normals(normals, elements, verts);

	Drawable* res = new Drawable();
	res->colours_enabled = true;
	res->normals_enabled = true;
	res->init(verts, (int)num_verts, colours, &elements[0], elements.size(), normals);

	return res;
}

TerrainGenerator::TerrainGenerator()
{
	x_points = 10;
	z_points = 10;

	x_world = 20.f;
	z_world = 20.f;

	altitude_variation_scale = 10;

	octaves = 4;
	perlin_scale = 4.f;
	perlin_freq = 3.f;
}


TerrainGenerator::~TerrainGenerator()
{
}
