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

void TerrainGenerator::calculate_normals(glm::vec3 * normals, std::vector<GLuint>* elements, glm::vec3* verts, glm::vec2 resolution)
{
	//std::cerr << "19n: " << normals[19].x << " " << normals[19].y << " " << normals[19].z << std::endl;
	GLuint element_pos = 0;
	glm::vec3 AB, AC, cross_product;

	// Loop through each triangle strip  
	for (GLuint y = 1; y < resolution.y - 2; y++)
	{
		// Loop along the strip
		for (GLuint tri = 0; tri < resolution.y * 2 - 2; tri++)
		{
			// Extract the vertex indices from the element array 
			GLuint v1 = elements->at(element_pos);
			GLuint v2 = elements->at(element_pos+1);
			GLuint v3 = elements->at(element_pos+2);

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
	}

	
	//std::cerr << "19n: " << normals[19].x << " " << normals[19].y << " " << normals[19].z << std::endl;
}


void TerrainGenerator::calculate_normals_sphere(glm::vec3 * normals, std::vector<GLuint>* elements, glm::vec3* verts, glm::vec2 resolution)
{
	//for testing
	for (GLuint element : *elements)
	{
		normals[element] = glm::vec3(0, 0, 0);
	}

	//south pole(WORKING)
	for (int i = 1; i <= resolution.x+1; i++)
	{
		glm::vec3 AB, AC, cross;
		AB = verts[i] - verts[0];
		int second = i+1>resolution.x+1 ? 2 : i+1;
		AC = verts[second] - verts[0];
		cross = glm::normalize(glm::cross(AB, AC));
		normals[0] += cross;
		normals[i] += cross;
		normals[second] += cross;
		std::cout << 0 << " " << second << " " << i << std::endl;
	}
	//1,11,10 are at the same point in space in this case so need to make sure their normals are the same
	//because 1 is start
	//11 is assigned same as start so the triangle strip 100% joins
	//10 is there because it aligned well in this specific case, but potentially it won't be aligned
	//this will need to be applied the same way to every triangle strip
	normals[(int)resolution.x] = normals[(int)resolution.x+1] = normals[1];


	////calculating normals for latitudes
	//for (int i = 1; i < resolution.y-2; i++)
	//{
	//	int a, b, c, d;
	//	for (int j = 0; j < resolution.x; j++)
	//	{
	//		//arrangement of points
	//		// **b---d** 
	//		// **|*/*|**
	//		// **a---c**
	//		//abd, adc

	//		//calculate participating indices for square
	//		a = 1 + i * (resolution.x + 1) + j;
	//		c = a+1;
	//		b = 1 + (i - 1) * (resolution.x+1) + j;
	//		d = b + 1;

	//		//calculate cross product
	//		glm::vec3 A = verts[elements->at(a)];
	//		glm::vec3 B = verts[elements->at(b)];
	//		glm::vec3 C = verts[elements->at(c)];
	//		glm::vec3 D = verts[elements->at(d)];
	//		glm::vec3 AB, AC, AD, cross1, cross2;

	//		AB = A - B;
	//		AC = A - C;
	//		AD = A - D;

	//		cross1 = glm::normalize(glm::cross(AB, AD));
	//		cross2 = glm::normalize(glm::cross(AD, AC));

	//		//apply cross products that are applicable to verts
	//		normals[elements->at(a)] += cross1 + cross2;
	//		normals[elements->at(d)] += cross1 + cross2;

	//		normals[elements->at(b)] += cross1;
	//		normals[elements->at(c)] += cross2;


	//		std::cout << a << " " << b << " " << d << std::endl;
	//		std::cout << a << " " << d << " " << c << std::endl << std::endl;
	//	}

	//	//calculate wrapped around (end of row to start of row)
	//	c = i * (resolution.x + 1) + 1;
	//	d = (i - 1) * (resolution.x + 1) + 1;

	//	//calculate cross product
	//	glm::vec3 A = verts[elements->at(a)];
	//	glm::vec3 B = verts[elements->at(b)];
	//	glm::vec3 C = verts[elements->at(c)];
	//	glm::vec3 D = verts[elements->at(d)];
	//	glm::vec3 AB, AC, AD, cross1, cross2;
	//	AB = A - B;
	//	AC = A - C;
	//	AD = A - D;

	//	cross1 = glm::normalize(glm::cross(AB, AD));
	//	cross2 = glm::normalize(glm::cross(AD, AC));

	//	normals[elements->at(a)] += cross1 + cross2;
	//	normals[elements->at(d)] += cross1 + cross2;

	//	normals[elements->at(b)] += cross1;
	//	normals[elements->at(c)] += cross2;

	//	std::cout << a << " " << b << " " << d << std::endl;
	//	std::cout << a << " " << d << " " << c << std::endl << std::endl;


	//}
	
	//north pole (WORKING)
	int num_verts = 2 + ((resolution.y - 2) * (resolution.x + 1));
	for (int i = num_verts - 2; i >= (num_verts - (resolution.x+2)); i--)
	{
		glm::vec3 AB, AC, cross;
		AB = verts[i] - verts[num_verts - 1];

		int second = i - 1 < num_verts - resolution.x - 2 ? num_verts-4 : i - 1;
		AC = verts[second] - verts[num_verts - 1];

		cross = glm::normalize(glm::cross(AB, AC));
		normals[num_verts - 1] += cross;
		normals[i] += cross;
		normals[second] += cross;
		
			
		std::cout << num_verts-1 << " " << second << " " << i << std::endl;
	}
	//same as for south pole
	 normals[num_verts-2] = normals[num_verts-3] = normals[num_verts-((int)resolution.x+1)-1];

	//////normalise normals
	//for (GLuint v = 0; v < 2 + ((resolution.y - 2) * (resolution.x+1)); v++)
	//{
	//	normals[v] = glm::normalize(normals[v]);
	//}

	
	//std::cerr << "19n: " << normals[19].x << " " << normals[19].y << " " << normals[19].z << std::endl;
}

Sphere* TerrainGenerator::create_terrain_on_sphere(Shader shader, int numlats, int numlongs, GLuint tex)
{
	Sphere* sphere = new Sphere(shader, tex);
	sphere->makeSphere(numlats, numlongs);

	this->num_longs = numlongs;
	this->num_lats = numlats;

	//glm::vec2 resolution(98, 100);
	//std::vector<glm::vec3> ridge = FeatureGenerator::generate_ridge_sphere(resolution, 1.01f, .9f, 0.45f);
	//resolution = glm::vec2(98, 100);
	//std::vector<glm::vec3> crater = FeatureGenerator::generate_crater(resolution, 0.95f, .5f);
	//
	//int maxid = 0;
	//glm::vec2 scale(1.f,1.f);
	//scale.x = 1.f;
	//scale.y = 1.f;
	//glm::vec2 shift(0,00);//x-sideways y-updown
	////needs ridges, resolution, scale
	//apply_terrain_feature_sphere(ridge, scale, shift, sphere);
	///*scale.x = .5f;
	//scale.y = .5f;
	//shift = glm::vec2(0, 000);
	//apply_terrain_feature_sphere(crater, scale, shift, sphere);*/
	//
	glm::vec3* temp = sphere->normals;
	calculate_normals_sphere(temp, &std::vector<GLuint>(sphere->indices, sphere->indices+sphere->num_indices), sphere->verts, glm::vec2(sphere->numlongs, sphere->numlats));
	sphere->normals = temp;
	sphere->reload_in_memory();
	return sphere;
}

void TerrainGenerator::apply_terrain_feature_sphere(std::vector<glm::vec3> feature, glm::vec2 scale, glm::vec2 shift, Sphere* sphere)
{
	std::vector<int> visited;
	for (glm::vec3 v : feature)
	{
		glm::vec3 vert = v;

		//feature resolution to sphere resolution
		glm::vec2 t_vert;
		t_vert.x = /*(sphere->numlongs-1)/ resolution.x **/ vert.x;
		t_vert.y = /*(sphere->numlats) / resolution.y **/ vert.z+1;//shift everything from 0 - x range to 1 - x+1

		t_vert.x *= scale.x;
		t_vert.y *= scale.y;

		t_vert.y = shift.y + t_vert.y;
		t_vert.x = shift.x + t_vert.x;

		//horizontal (more maths to allow to wrap around
		t_vert.x = t_vert.x < 0 ? ((int)sphere->numlongs+1 - (int)t_vert.x) % (int)sphere->numlongs+1 : t_vert.x;
		t_vert.x = t_vert.x > sphere->numlongs+1 ? (int)t_vert.x % (int)sphere->numlongs : t_vert.x;
		//vertical (shouldn't wrap around
		t_vert.y = t_vert.y <= 0 ? 1 : t_vert.y;
		t_vert.y = t_vert.y >= sphere->numlats ?  sphere->numlats : t_vert.y;

		int id = std::round(std::floor(t_vert.y-1) * (sphere->numlongs+1) + t_vert.x+1);
		if(id > 9900)
			std::cout << id << std::endl;
		if (id <= 0)
			std::cout << id << std::endl;

		//safeguard against exagerrating features when scaling down (same point isn't moved twice)
		bool vis = false;
		for (int i : visited)
		{
			if (i == id)
			{
				vis = true;
				break;
			}
		}
		if (vis)
			continue;


		/*	if (vert.y == 2.f)
				std::cout << "2" << std::endl;*/
		glm::vec3 to_center(sphere->verts[id]);
		float cur_length = glm::length(to_center);
		float target_length = cur_length * vert.y;
		sphere->verts[id] = { target_length * to_center.x / cur_length, target_length * to_center.y / cur_length, target_length * to_center.z / cur_length };

		visited.push_back(id);
	}
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
	calculate_normals(normals, elements, verts, glm::vec2(x_points,z_points));
	
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
