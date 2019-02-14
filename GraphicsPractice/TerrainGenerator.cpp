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
	//for testing (so it's easy to spot verts that weren't covered by stages below
	/*for (GLuint element : *elements)
	{
		normals[element] = glm::vec3(0, 0, 0);
	}*/

	/*STAGE ONE*/
	//south pole
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
		//std::cout << 0 << " " << second << " " << i << std::endl;
	}
	

	/*STAGE TWO*/
	//calculating normals for latitudes
	for (int i = 1; i < resolution.y-2; i++)
	{
		//std::cout << "i: " << i << std::endl;
		int a, b, c, d;
		for (int j = 0; j < resolution.x; j++)
		{
			int cur_elem = 1 + i * (resolution.x + 1) + j;
			int b = cur_elem - (resolution.x + 1);
			int d = b + 1;
			int c = cur_elem + 1;

			//verts
			glm::vec3 A = verts[cur_elem];
			glm::vec3 B = verts[b];
			glm::vec3 C = verts[c];
			glm::vec3 D = verts[d];
			
			glm::vec3 AB, AC, AD, cross1, cross2;
			AB = A - B;
			AC = A - C;
			AD = A - D;

			cross1 = glm::normalize(glm::cross(AD, AB));
			cross2 = glm::normalize(glm::cross(AC, AD));

			normals[cur_elem] += cross1 + cross2;
			normals[d] += cross1 + cross2;
			normals[b] += cross1;
			normals[c] += cross2;
		}		
	}

	/*STAGE THREE*/
	//north pole
	int num_verts = 2 + ((resolution.y - 2) * (resolution.x + 1));
	for (int i = num_verts - 2; i >= (num_verts - (resolution.x + 2)); i--)
	{
		glm::vec3 AB, AC, cross;
		AB = verts[i] - verts[num_verts - 1];

		int second = i - 1 < num_verts - resolution.x - 2 ? num_verts - 4 : i - 1;
		AC = verts[second] - verts[num_verts - 1];

		cross = glm::normalize(glm::cross(AB, AC));
		normals[num_verts - 1] += cross;
		normals[i] += cross;
		normals[second] += cross;
	}


	/*STAGE FOUR*/
	//1,11,10 are at the same point in space (assuming 10 longs and 10 lats sphere) in this case so need to make sure their normals are the same
	//because 1 is start
	//11 is assigned same as 1 so the triangle strip 100% joins
	//10 is there because it aligned well in this specific case, but potentially it might not be aligned
	//this will need to be applied the same way to every triangle strip
	normals[(int)resolution.x] = normals[(int)resolution.x + 1] = normals[1];
	//MAKE ALL THREE NORMALS AT END OF EACH LAT EQUAL
	for (int i = 1; i < resolution.y - 2; i++)
	{	
		int line_start = 1 + i * ((int)resolution.x + 1);
		int line_last = line_start + resolution.x;
		normals[line_last] = normals[line_last - 1] = normals[line_start];
	}
	//same as for south pole
	 normals[num_verts-2] = normals[num_verts-3] = normals[num_verts-((int)resolution.x+1)-1];
}

Sphere* TerrainGenerator::create_terrain_on_sphere(Shader shader, int numlats, int numlongs, GLuint tex)
{
	Sphere* sphere = new Sphere(shader, tex);
	sphere->makeSphere(numlats, numlongs);

	this->num_longs = numlongs;
	this->num_lats = numlats;

	glm::vec2 resolution(1000, 1000);
	std::vector<glm::vec3> ridge = FeatureGenerator::generate_ridge_sphere(resolution, 1.01f, .99f, 0.35f);
	std::vector<glm::vec3> tiny_ridge = FeatureGenerator::generate_ridge_sphere(resolution, 1.005f, .99f, 0.25f);
	//resolution = glm::vec2(98, 100);
	//std::vector<glm::vec3> crater = FeatureGenerator::generate_crater(resolution, 0.95f, .5f);
	
	int maxid = 0;
	glm::vec2 scale;
	scale.x = .005f;
	scale.y = .1f;
	glm::vec2 shift(100,300);//x-sideways y-updown
	//needs ridges, resolution, scale
	for (int i = 0; i < 5; i++)
	{
		apply_terrain_feature_sphere(tiny_ridge, scale, shift, sphere, resolution);
		shift.x += 5;
	}

	float angle = 30;
	scale.x = 0.015f;
	shift.x -= 30;
	for (int i = 0; i < 5; i++)
	{
		apply_terrain_feature_sphere(ridge, scale, shift, sphere, resolution, angle);
		shift.x -= 20;
	}
	/*scale.x = .5f;
	scale.y = .5f;
	shift = glm::vec2(0, 000);
	apply_terrain_feature_sphere(crater, scale, shift, sphere);*/
	
	glm::vec3* temp = sphere->normals;
	calculate_normals_sphere(temp, &std::vector<GLuint>(sphere->indices, sphere->indices+sphere->num_indices), sphere->verts, glm::vec2(sphere->numlongs, sphere->numlats));
	sphere->normals = temp;
	sphere->reload_in_memory();
	return sphere;
}

void TerrainGenerator::apply_terrain_feature_sphere(std::vector<glm::vec3> feature, glm::vec2 scale, glm::vec2 shift, Sphere* sphere, glm::vec2 feature_resolution, float rotation_angle)
{
	//std::vector<int> visited;

	int* visited = new int[sphere->num_verts];
	for (int i = 0; i < sphere->num_verts; i++)
		visited[i] = 0;

	int a = 0;
	
	for (glm::vec3 v : feature)
	{
		glm::vec3 vert = v;

		//feature resolution to sphere resolution
		glm::vec2 t_vert;
		t_vert.x = /*(sphere->numlongs-1)/ resolution.x **/ vert.x;
		t_vert.y = /*(sphere->numlats) / resolution.y **/ vert.z;//shift everything from 0 - x range to 1 - x+1

		//convert feature coords to sphere coords?
		glm::vec2 vert_on_sphere;
		vert_on_sphere.x = t_vert.x / feature_resolution.y * (float)sphere->numlongs;
		vert_on_sphere.y = t_vert.y / feature_resolution.x * (sphere->numlats - 3);

		vert_on_sphere.x *= scale.x;
		vert_on_sphere.y *= scale.y;

		//rotate
		int terr_rotatedx = std::floor((float)vert_on_sphere.x * glm::cos(glm::radians(rotation_angle)) - (float)vert_on_sphere.y * glm::sin(glm::radians(rotation_angle)));
		int terr_rotatedz = std::floor((float)vert_on_sphere.y * glm::cos(glm::radians(rotation_angle)) + (float)vert_on_sphere.x * glm::sin(glm::radians(rotation_angle)));
		vert_on_sphere.x = terr_rotatedx;
		vert_on_sphere.y = terr_rotatedz;

		vert_on_sphere.y += shift.y;
		vert_on_sphere.x += shift.x;

		//horizontal (more maths to allow to wrap around)//IF X GOES BELOW 0, ROTATED RIDGE REFLECTS OFF 0?!
		int nl = sphere->numlongs;
		vert_on_sphere.x = vert_on_sphere.x < 0 ? nl - std::abs(((int)vert_on_sphere.x % nl)) : vert_on_sphere.x;
		vert_on_sphere.x = vert_on_sphere.x >= nl-1 ? (int)vert_on_sphere.x % nl : vert_on_sphere.x;
		//vertical (shouldn't wrap around)
		vert_on_sphere.y = vert_on_sphere.y <= 0 ? 1 : vert_on_sphere.y;
		vert_on_sphere.y = vert_on_sphere.y >= sphere->numlats-3 ? sphere->numlats-4 : vert_on_sphere.y;

		//this ensures that rows don't get shifted and feature is properly translated
		/*if (vert_on_sphere.x >= sphere->numlongs || 
			vert_on_sphere.y >= sphere->numlats-3 || 
			vert_on_sphere.x < 0 || 
			vert_on_sphere.y < 0)
			continue;*/

		int id = std::round(std::floor(vert_on_sphere.y) * (sphere->numlongs+1) + std::floor(vert_on_sphere.x)) + 1;
		//if(id >= 90)
		//	std::cout << id << std::endl;
		//if (id <= 0)
		//	std::cout << id << std::endl;

		//safeguard against exagerrating features when scaling down (same point isn't moved twice)
		/*bool vis = false;
		for (int i : visited)
		{
			if (i == id)
			{
				vis = true;
				break;
			}
		}
		if (vis)
			continue;*/

		if (visited[id] == 1)
			continue;


		/*	if (vert.y == 2.f)
				std::cout << "2" << std::endl;*/
		glm::vec3 to_center(sphere->verts[id]);
		float cur_length = glm::length(to_center);
		float target_length = cur_length * vert.y;
		sphere->verts[id] = { target_length * to_center.x, target_length * to_center.y, target_length * to_center.z };
		//visited.push_back(id);
		visited[id] = 1;
		
		/*if(a%100==0)
			std::cout << a << std::endl;
		a++;*/
	}



		/*std::vector<int> visited_indices;
	//for every vertical row, scan all elements

	glm::vec2 center = glm::vec2(feature_resolution.x / 2, feature_resolution.y / 2);

	for (glm::vec3 vert : feature)
	{
		static int prev_rotatex = 0;

		//get coordinates of that vert on terrain
		int terr_x = (vert.x * feature_scale.x + feature_position.x);
		int terr_z = (vert.z * feature_scale.y + feature_position.y);
		int terr_x = ((vert.x-center.x) * feature_scale.x + feature_position.x);
		int terr_z = ((vert.z-center.y) * feature_scale.y + feature_position.y);
		int terr_x = vert.x - center.x;
		int terr_z = vert.z - center.y;

		//attempt rotation
		int terr_rotatedx = std::floor((float)terr_x * glm::cos(glm::radians(rotation)) - (float)terr_z * glm::sin(glm::radians(rotation)));
		int terr_rotatedz = std::floor((float)terr_z * glm::cos(glm::radians(rotation)) + (float)terr_x * glm::sin(glm::radians(rotation)));

		terr_x += terr_rotatedx;
		terr_z += terr_rotatedx;
		terr_x = terr_rotatedx + center.x;
		terr_z = terr_rotatedx + center.y;



		//this ensures that rows don't get shifted and feature is properly translated
		if (terr_x >= terrain_resolution.x || terr_z >= terrain_resolution.y || terr_x < 0 || terr_z < 0)
			continue;

		int terr_ind = terr_z * terrain_resolution.y + terr_x;

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
		////check if index was used and if it was, go to next iteration
		//	//this avoids adding values multiple times to same index
		//bool done = false;
		//for (int i = 0; i < visited_indices.size(); i++)
		//	if (visited_indices.at(i) == terr_ind)
		//	{
		//		done = true;
		//		break;
		//	}

		//if (done)
		//	continue;

		//just a safeguard
		if (terr_ind < 0)
			continue;
		terrain[terr_ind].y += vert.y;
		visited_indices.push_back(terr_ind);
	}
}*/
	

	//join last vertex in each line to 0th vertex of line so there is never a gap
	for (int i = 1; i < sphere->numlats - 2; i++)
	{
		int line_start = 1 + i * ((int)sphere->numlongs + 1);
		int line_last = line_start + sphere->numlongs;

		sphere->verts[line_start] = sphere->verts[line_last];
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
