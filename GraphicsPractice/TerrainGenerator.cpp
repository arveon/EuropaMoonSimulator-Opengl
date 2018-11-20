#include "TerrainGenerator.h"



GLfloat * TerrainGenerator::calculate_noise(GLuint x_size, GLuint z_size, GLuint octaves, GLuint frequency, GLuint scl)
{
	int num_verts = x_size * z_size;
	GLfloat* noise = new GLfloat[num_verts * octaves];
	for (int i = 0; i < num_verts*octaves; i++)
		noise[i] = 0;

	GLfloat xfactor = 1.f / (x_size - 1);//the amount noise will be moved per unit
	GLfloat zfactor = 1.f / (z_size - 1);
	GLfloat freq = frequency;
	GLfloat scale = scl;

	for (GLuint row = 0; row < z_size; row++)
	{
		for (GLuint col = 0; col < x_size; col++)
		{
			//get coords of where noise will be sampled
			GLfloat x = xfactor * col;
			GLfloat z = zfactor * row;

			GLfloat sum = 0;
			GLfloat cur_scale = scale;
			GLfloat cur_freq = freq;

			//sum for each octave
			for (GLuint oct = 0; oct < octaves; oct++)
			{
				glm::vec2 point(x*cur_freq, z*cur_freq);
				GLfloat val = glm::perlin(point) / cur_scale;
				sum += val;

				GLfloat res = (sum + 1.f) / 2.f;
				noise[(row*x_size + col)*octaves + oct] = res;

				cur_freq *= 2.f;
				cur_scale *= scale;
			}
		}
	}

	return noise;
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
		//glm::vec3 new_n = glm::vec3(normals[v * 3], normals[v * 3 + 1], normals[v * 3 + 2]);
		normals[v] = glm::normalize(normals[v]);
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
			GLfloat height = noise[(x*z_points + z) * octaves + octaves - 1] + 0.5f;
			int cur_index = x*x_points + z;
			verts[cur_index] = glm::vec3(xpos, height, zpos);//x
			normals[cur_index] = glm::vec3(0, 1.f, 0);//x
			colours[cur_index] = glm::vec4(1.f-height, 1.f-height, 1.f-height, 1.0f);
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

	calculate_normals(normals, elements, verts);

	Drawable* res = new Drawable();
	res->colours_enabled = true;
	res->init(verts, (int)num_verts, colours, &elements[0], elements.size(), normals);

	return res;
}

TerrainGenerator::TerrainGenerator()
{
	x_points = 50;
	z_points = 50;

	x_world = 10.f;
	z_world = 10.f;

	octaves = 4;
	perlin_scale = 2.f;
	perlin_freq = 1.f;
}


TerrainGenerator::~TerrainGenerator()
{
}
