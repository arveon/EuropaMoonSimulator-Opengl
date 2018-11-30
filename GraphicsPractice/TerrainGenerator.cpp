/*
Responsible for generating a chunk of terrain and placing features on it.
by Aleksejs Loginovs - Nov 2018
basic terrain generation parts adapted from Iain Martin's basic terrain generation example
*/


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

Terrain* TerrainGenerator::create_terrain(int xpoints, int zpoints, float x_world, float z_world, GLuint perlin_freq, GLuint scl, GLuint octaves)
{
	std::cerr << "Generating terrain:" << std::endl;
	this->x_points = xpoints;
	this->z_points = zpoints;
	this->x_world = x_world;
	this->z_world = z_world;
	this->perlin_freq = perlin_freq;
	this->octaves = octaves;
	this->perlin_scale = scl;


	GLuint num_verts = x_points * z_points;

	glm::vec3* verts = new glm::vec3[num_verts];
	glm::vec3* normals = new glm::vec3[num_verts];
	glm::vec4* colours= new glm::vec4[num_verts];
	glm::vec2* texcoords = new glm::vec2[num_verts];

	//calculate noise
	//allocate and set to 0
	GLfloat* noise = TerrainGenerator::calculate_noise(x_points, z_points, octaves, perlin_freq, perlin_scale);
	glm::vec2 ridge_resolution = glm::vec2(x_points, z_points);

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

	std::vector<GLuint>* elements = new std::vector<GLuint>();
	//calculate vertex order into element array
	for (GLuint x = 0; x < x_points - 1; x++)
	{
		GLuint top = x * z_points;//start of cur row
		GLuint bottom = top + z_points;//start of next row
		for (GLuint z = 0; z < z_points; z++)
		{
			elements->push_back(top++);
			elements->push_back(bottom++);
		}
	}

	scale_heights(0, 4, verts, num_verts);
	scale_colours(0, 1, colours, num_verts);
	
#pragma region ridges
	//generate a few ridges with random heights
	std::vector<std::vector<glm::vec3>> ridges;
	std::cerr << "Generating ridges:" << std::endl;
	int num_ridges = 20;
	for (int i = 0; i < num_ridges; i++)
	{
		std::cerr << i << " of " << num_ridges << std::endl;
		ridges.push_back(FeatureGenerator::generate_ridge(ridge_resolution, (rand() % 100 + 200) / 100, -(rand() % 100 + 200) / 100.f, 0.5));
	}
	//apply random ridges
	std::cerr << "Applying ridges" << std::endl;
	int num_ridges_placed = 20;
	for (int i = 0; i < num_ridges_placed; i++)
	{
		std::cerr << i << " of " << num_ridges_placed << std::endl;
		int type = 0;
		glm::vec2 pos;
		glm::vec2 scale;

		type = rand() % 10;

		if (type >= 5)
		{
			//large
			scale = glm::vec2(1, (rand() % 20 + 10) / 100.f);
		}
		else
		{
			//medium size
			scale = glm::vec2(1, (rand() % 30 + 5) / 100.f);
		}
		//else
		//{
		//	//small
		//	scale = glm::vec2(1, (rand() % 30 + 10) / 100.f);
		//}

		pos = glm::vec2(x_points/2, rand() % z_points);

		apply_terrain_feature(ridges.at(rand()%ridges.size()), verts, pos, scale, ridge_resolution, glm::vec2(x_points, z_points), 0);
	}
#pragma endregion
#pragma region craters
	std::cerr << "Generating craters:" << std::endl;
	//generate a few craters with random heights
	std::vector<std::vector<glm::vec3>> craters;
	int num_craters = 10;
	for (int i = 0; i < num_craters; i++)
	{
		std::cerr << i << " of " << num_craters << std::endl;
		bool positive = rand() % 2 == 1;
		if(positive)
			craters.push_back(FeatureGenerator::generate_crater(ridge_resolution, (rand()%100 + 200) /100.f, 0.5));
		else
			craters.push_back(FeatureGenerator::generate_crater(ridge_resolution, -(rand() % 100 + 200) / 100.f, 0.5));
	}
	//apply random craters
	int num_craters_applied = 10;
	std::cerr << "Applying craters" << std::endl;
	for (int i = 0; i < num_craters_applied; i++)
	{
		std::cerr << i << " of " << num_craters_applied << std::endl;
		int type = 0;
		glm::vec2 pos;
		glm::vec2 scale;

		type = rand() % 10;

		if (type >= 9)
		{
			//large
			scale = glm::vec2((rand()%30 + 20.f)/100.f, (rand() % 30 + 20.f) / 100.f);
		}
		else if (type > 7)
		{
			//medium size
			scale = glm::vec2((rand() % 15 + 15.f) / 100.f, (rand() % 15 + 15.f) / 100.f);
		}
		else
		{
			//small
			scale = glm::vec2((rand() % 10 + 10.f) / 100.f, (rand() % 10 + 10.f) / 100.f);
		}

		//scale = glm::vec2(1, 1);
		pos = glm::vec2(rand()%x_points, rand()%z_points);

		apply_terrain_feature(craters.at(rand() % craters.size()), verts, pos, scale, ridge_resolution, glm::vec2(x_points, z_points), 0, true);
	}

	//put a height in the centre
	std::vector<glm::vec3> central_crater = FeatureGenerator::generate_crater(ridge_resolution, 3.0f, 0.5f);
	glm::vec2 scale = glm::vec2(0.1, 0.1);
	glm::vec2 pos = glm::vec2(x_points / 2, z_points / 2);
	apply_terrain_feature(central_crater, verts, pos, scale, ridge_resolution, glm::vec2(x_points, z_points), 0, true);


#pragma endregion

	std::cerr << "Calculating terrain normals" << std::endl;
	calculate_normals(normals, elements, verts);
	
	Terrain* temp = new Terrain(x_points, verts, (int)num_verts, colours, &(elements->at(0)), elements->size(), normals,texcoords, terrain_texture);
	return temp;
}

void TerrainGenerator::apply_terrain_feature(std::vector<glm::vec3> feature, glm::vec3 * terrain, glm::vec2 feature_position, glm::vec2 feature_scale, glm::vec2 feature_resolution, glm::vec2 terrain_resolution, float rotation,bool is_crater)
{
	bool** visited = new bool*[terrain_resolution.y];
	for (int i = 0; i < terrain_resolution.y; i++)
	{
		visited[i] = new bool[terrain_resolution.x];
		for (int j = 0; j < terrain_resolution.x; j++)
			visited[i][j] = false;
	}

	glm::vec2 center = glm::vec2(feature_resolution.x / 2, feature_resolution.y / 2);

	for (glm::vec3 vert : feature)
	{
		static int prev_rotatex = 0;

		//get coordinates of that vert on terrain
		int terr_x = (vert.x * feature_scale.x + feature_position.x - center.x*feature_scale.x);
		int terr_z = (vert.z * feature_scale.y + feature_position.y - center.y*feature_scale.y);

		/*int terr_rotatedx = std::floor((float)terr_x * glm::cos(glm::radians(rotation)) - (float)terr_z * glm::sin(glm::radians(rotation)));
		int terr_rotatedz = std::floor((float)terr_z * glm::cos(glm::radians(rotation)) + (float)terr_x * glm::sin(glm::radians(rotation)));*/

		//this ensures that rows don't get shifted and feature is properly translated
		if (terr_x >= terrain_resolution.x || terr_z >= terrain_resolution.y || terr_x < 0 || terr_z < 0 /*||
			terr_rotatedx >= terrain_resolution.x || terr_rotatedz >= terrain_resolution.y || terr_rotatedx < 0 || terr_rotatedz < 0*/)
			continue;
		
		if (terr_x >= terrain_resolution.x || terr_z >= terrain_resolution.y || terr_x < 0 || terr_z < 0 )
			continue;

		int terr_ind = terr_z * terrain_resolution.y + terr_x;

		//int y = (int)std::floor(terr_ind / terrain_resolution.y);
		//int x = terr_ind % (int)terrain_resolution.x;

		//y = std::floor(vert.z);
		//x = std::floor(vert.x);
		if (visited[terr_z][terr_x])
			continue;
		else
			visited[terr_z][terr_x] = true;

		//if (done)
		//	continue;

		//just a safeguard
		if (terr_ind < 0)
			continue;

 		terrain[terr_ind].y += vert.y;

		if(is_crater)
			terrain[terr_ind].y = vert.y;

		/*visited_indices.push_back(terr_ind);*/
	}

	for (int i = 0; i < terrain_resolution.y; i++)
	{
		delete visited[i];
	}
	delete visited;
}

TerrainGenerator::TerrainGenerator()
{
	x_points = 50;
	z_points = 50;

	x_world = 50.f;
	z_world = 50.f;

	octaves = 8;
	perlin_scale = 14.f;
	perlin_freq = 4.f;
}


TerrainGenerator::~TerrainGenerator()
{
}
