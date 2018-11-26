#include "TerrainGenerator.h"



GLfloat * TerrainGenerator::calculate_noise(GLuint x_size, GLuint z_size, GLuint octaves, GLuint frequency, GLuint scl)
{
	GLuint num_verts = x_size * z_size;
	GLfloat* noise = new GLfloat[num_verts * octaves];
	for (GLuint i = 0; i < num_verts*octaves; i++)
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

void TerrainGenerator::calculate_normals(glm::vec3 * normals, std::vector<GLuint>* elements, glm::vec3* verts)
{
	GLuint element_pos = 0;
	glm::vec3 AB, AC, cross_product;

	// Loop through each triangle strip  
	for (GLuint x = 0; x < x_points - 1; x++)
	{
		// Loop along the strip
		for (GLuint tri = 0; tri < z_points * 2 - 2; tri++)
		{
			// Extract the vertex indices from the element array 
			GLuint v1 = elements->at(element_pos);
			GLuint v2 = elements->at(element_pos+1);
			GLuint v3 = elements->at(element_pos + 2);

			// Define the two vectors for the triangle
			AB = verts[v2] - verts[v1];
			AC = verts[v3] - verts[v1];

			// Calculate the cross product
			cross_product = glm::normalize(glm::cross(AC, AB));

			// Add this normal to the vertex normal for all three vertices in the triangle
			normals[v1] += cross_product;
			normals[v2] += cross_product;
			normals[v3] += cross_product;

			/*if (v1 == 19 || v2 == 19 || v3 == 19)
			{
				std::cerr << v1 << " " << v2 << " " << v3 << std::endl;
				std::cerr << "19n: " << normals[19].x << " " << normals[19].y << " " << normals[19].z << std::endl;
			}*/

			// Move on to the next vertex along the strip
			element_pos++;
		}

		// Jump past the last two element positions to reach the start of the strip
		element_pos += 2;
	}
	//normalise normals
	for (GLuint v = 0; v < x_points*z_points; v++)
	{
		normals[v] = glm::normalize(normals[v]);
		//normals[v].x = -normals[v].x;
		//std::cerr << v << ": " << normals[v].x << " " << normals[v].y << " " << normals[v].z << std::endl;
	}

	
	std::cerr << "19n: " << normals[19].x << " " << normals[19].y << " " << normals[19].z << std::endl;
}

Terrain* TerrainGenerator::create_terrain()
{
	GLuint num_verts = x_points * z_points;

	glm::vec3* verts = new glm::vec3[num_verts];
	glm::vec3* normals = new glm::vec3[num_verts];
	glm::vec4* colours= new glm::vec4[num_verts];
	glm::vec2* texcoords = new glm::vec2[num_verts];

	//calculate noise
	//allocate and set to 0
	GLfloat* noise = TerrainGenerator::calculate_noise(x_points, z_points, octaves, perlin_freq, perlin_scale);
	glm::vec2 ridge_resolution = glm::vec2(x_points, z_points);
	glm::vec3* ridge = Ridge::generate_ridge(ridge_resolution, 0.1, -0.1, 0.5);

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
			texcoords[cur_index] = glm::vec2(xpos / x_world,zpos/z_world);

		}
	}
	apply_terrain_feature(ridge, verts, glm::vec2(10, 15), glm::vec2(0.25,0.2), ridge_resolution, glm::vec2(x_points,z_points), 0);


	std::vector<GLuint>* elements = new std::vector<GLuint>();
	//calculate vertex order into element array
	for (GLuint x = 0; x < x_points-1; x++)
	{
		GLuint top = x * z_points;//start of cur row
		GLuint bottom = top + z_points;//start of next row
		for (GLuint z = 0; z < z_points; z++)
		{
			elements->push_back(top++);
			elements->push_back(bottom++);
		}
	}

	scale_heights(0, 3, verts, num_verts);
	scale_colours(0, 1, colours, num_verts);
	calculate_normals(normals, elements, verts);
	
	Terrain* temp = new Terrain(x_points, verts, (int)num_verts, colours, &(elements->at(0)), elements->size(), normals,texcoords, terrain_texture);
	return temp;
}

void TerrainGenerator::apply_terrain_feature(glm::vec3 * feature, glm::vec3 * terrain, glm::vec2 feature_position, glm::vec2 feature_scale, glm::vec2 feature_resolution, glm::vec2 terrain_resolution, float rotation)
{
	std::vector<int> visited_indices;
	//for every vertical row, scan all elements
	for (int z = 0; z < feature_resolution.y; z+=round(1/feature_scale.y))
	{
		for (int x = 0; x < feature_resolution.x; x+=round(1/feature_scale.x))
		{
			int terr_x = (x * feature_scale.x + feature_position.x);
			int terr_z = (z * feature_scale.y + feature_position.y);

			int terr_rotatedx = std::floor((float)terr_x * glm::cos(glm::radians(rotation)) - (float)terr_z * glm::sin(glm::radians(rotation)));
			int terr_rotatedz = std::floor((float)terr_z * glm::cos(glm::radians(rotation)) + (float)terr_x * glm::sin(glm::radians(rotation)));

			//this ensures that rows don't get shifted and feature is properly translated
			if (terr_x >= terrain_resolution.x || terr_z >= terrain_resolution.y || terr_x < 0 || terr_z < 0 ||
				terr_rotatedx >= terrain_resolution.x || terr_rotatedz >= terrain_resolution.y || terr_rotatedx < 0 || terr_rotatedz < 0)
				continue;

			int terr_ind = terr_rotatedz * terrain_resolution.y + terr_rotatedx;


			//check if index was used and if it was, go to next iteration
			//this avoids adding values multiple times to same index
			bool done = false;
			for (int i = 0; i < visited_indices.size(); i++)
				if (visited_indices.at(i) == terr_ind)
				{
					done = true;
					break;
				}

			if (done)
				continue;

			int feature_ind = z * feature_resolution.y + x;

			//just a safeguard
			if (terr_ind < 0 || feature_ind < 0)
				continue;

			terrain[terr_ind].y += feature[feature_ind].y;			
			visited_indices.push_back(terr_ind);
		}
	}





	////generate vertices
	//for (GLuint x = 0; x < x_points; x++)
	//{

	//	for (GLuint z = 0; z < z_points; z++)
	//	{
	//		//calculate feature index at that terrain position
	//		int feature_index = x * x_points + z;

	//		int cur_index = x * x_points + z;
	//		terrain[cur_index].y += feature[feature_index].y;

	//		//std::cerr << cur_index << "v: " << verts[cur_index].x << " " << verts[cur_index].y << " " << verts[cur_index].z << std::endl;
	//	}
	//}


}

TerrainGenerator::TerrainGenerator()
{
	x_points = 50;
	z_points = 50;

	x_world = 20.f;
	z_world = 20.f;

	altitude_variation_scale = 10;

	octaves = 8;
	perlin_scale = 4.f;
	perlin_freq = 2.f;
}


TerrainGenerator::~TerrainGenerator()
{
}
