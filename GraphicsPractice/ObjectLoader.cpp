/*	by Aleksejs Loginovs - November 2018
	adapted from Iain Martin's object loading example code
*/
#include "ObjectLoader.h"



Drawable * ObjectLoader::load_object(std::string obj_path, Shader shader, GLuint tex_id)
{
	std::cerr << "Loading obj: " << obj_path << std::endl;
	Drawable* res = new Drawable();
	tinyobj::attrib_t obj;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	int num_verts, num_normals, num_tex_coords, num_colours = 0;

	glm::vec4 *colors = nullptr;
	glm::vec3 *vertices, *normals;
	glm::vec2 *texcoords = nullptr;
	vertices = normals= nullptr;
	GLuint *indices = nullptr;


	std::string error, warning;
	bool ret = tinyobj::LoadObj(&obj, &shapes, &materials, &warning, &error, obj_path.c_str());
	if (!error.empty()) 
		std::cout << error << std::endl;
	if (!warning.empty())
		std::cout << warning << std::endl;

	
	num_verts = obj.vertices.size() / 3;
	num_normals = obj.normals.size() / 3;
	num_tex_coords = obj.texcoords.size() / 2;
	num_colours = obj.colors.size() / 3;

	//if there are no vertices defined, no point in loading anything else
	if (num_verts <= 0)
	{
		std::cout << "No vertex coords." << std::endl;
		return nullptr;
	}

	//copy vertex data to array
	vertices = new glm::vec3[obj.vertices.size()];
	//std::copy(obj.vertices.begin(), obj.vertices.end(), vertices);
	int counter = 0;
	for (unsigned int i = 0; i < obj.vertices.size(); i+=3)
	{
		vertices[counter] = glm::vec3(obj.vertices[i], obj.vertices[i + 1], obj.vertices[i + 2]);
		counter++;
	}


	//copy stuff if present
	if (num_normals <= 0)
	{
		std::cout << "No normals." << std::endl;
		res->tex_enabled = false;
	}
	else
	{
		normals = new glm::vec3[obj.normals.size()];
		counter = 0;
		for (unsigned int i = 0; i < obj.normals.size(); i += 3)
		{
			normals[counter] = glm::vec3(obj.normals[i], obj.normals[i + 1], obj.normals[i + 2]);
			counter++;
		}
		res->normals_enabled = true;
	}

	if (num_tex_coords <= 0)
	{
		std::cout << "No tex coords." << std::endl;
		res->tex_enabled = false;
	}
	else
	{
		res->texture_id = tex_id;
		texcoords = new glm::vec2[obj.texcoords.size()];
		int counter = 0;
		for (unsigned int i = 0; i < obj.texcoords.size(); i += 2)
		{
			texcoords[counter] = glm::vec2(obj.texcoords[i], obj.texcoords[i + 1]);
			counter++;
		}

		res->tex_enabled = true;
	}

	if (num_colours <= 0)
	{
		std::cout << "No colours." << std::endl;
		res->tex_enabled = false;
	}
	else
	{
		colors = new glm::vec4[obj.colors.size()];
		counter = 0;
		for (unsigned int i = 0; i < obj.colors.size() - 3; i += 4)
		{
			colors[counter] = glm::vec4(obj.colors[i], obj.colors[i + 1], obj.colors[i + 2], obj.colors[i + 3]);
			counter++;
		}
		res->colours_enabled = true;
		//std::copy(obj.colors.begin(), obj.colors.end(), colors);
	}

	int num_indices;
	parse_indices(shapes, &indices, &num_indices);
	res->init(shader, vertices, obj.vertices.size(), colors, indices, num_indices, normals, texcoords);
	return res;
}

Sphere * ObjectLoader::load_object_sphere(std::string obj_path, Shader shader)
{
	std::cerr << "Loading obj: " << obj_path << std::endl;
	Sphere* res = new Sphere(shader);
	tinyobj::attrib_t obj;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	int num_verts, num_normals, num_tex_coords, num_colours = 0;

	glm::vec3 *colors = nullptr;
	glm::vec3 *vertices, *normals;
	glm::vec2 *texcoords = nullptr;
	vertices = normals = nullptr;
	GLuint *indices = nullptr;


	std::string error, warning;
	bool ret = tinyobj::LoadObj(&obj, &shapes, &materials, &warning, &error, obj_path.c_str());
	if (!error.empty())
		std::cerr << error << std::endl;
	if (!warning.empty())
		std::cerr << warning << std::endl;

	num_verts = obj.vertices.size() / 3;
	num_normals = obj.normals.size() / 3;
	num_tex_coords = obj.texcoords.size() / 2;
	num_colours = obj.colors.size() / 3;
	
	res->num_verts = num_verts;
	res->drawmode = 1;
	res->numlongs = (1+std::sqrt(num_verts*4 + 5))/2;
	res->numlats = res->numlongs;
	

	//if there are no vertices defined, no point in loading anything else
	if (num_verts <= 0)
	{
		std::cout << "No vertex coords." << std::endl;
		return nullptr;
	}

	//copy vertex data to array
	vertices = new glm::vec3[obj.vertices.size()];
	//std::copy(obj.vertices.begin(), obj.vertices.end(), vertices);
	int counter = 0;
	std::cout << "Processing vertex positions..." << std::endl;
	for (unsigned int i = 0; i < obj.vertices.size(); i += 3)
	{
		
		vertices[counter] = glm::vec3(obj.vertices[i], obj.vertices[i + 1], obj.vertices[i + 2]);
		counter++;
	}

	//copy stuff if present
	if (num_normals <= 0)
	{
		std::cout << "No normals." << std::endl;
		res->tex_enabled = false;
	}
	else
	{
		normals = new glm::vec3[obj.normals.size()];
		counter = 0;
		std::cout << "Processing vertex normals..." << std::endl;
		for (unsigned int i = 0; i < obj.normals.size(); i += 3)
		{
			normals[counter] = glm::vec3(obj.normals[i], obj.normals[i + 1], obj.normals[i + 2]);
			counter++;
		}
		res->normals_enabled = true;
	}

	if (num_tex_coords <= 0)
	{
		std::cout << "No tex coords." << std::endl;
		res->tex_enabled = false;
	}
	else
	{
		texcoords = new glm::vec2[obj.texcoords.size()];
		int counter = 0;
		std::cout << "Processing texture coordinates..." << std::endl;
		for (unsigned int i = 0; i < obj.texcoords.size(); i += 2)
		{
			texcoords[counter] = glm::vec2(obj.texcoords[i], obj.texcoords[i + 1]);
			counter++;
		}

		res->tex_enabled = true;
	}

	res->gen_buffers();
	res->colours_enabled = false;
	res->set_normals(normals);
	res->set_verts(vertices);
	res->set_texcoords(texcoords);
	res->produce_indices();
	res->reload_in_memory();
	return res;
}

void ObjectLoader::parse_indices(std::vector<tinyobj::shape_t> shapes, GLuint** ind, int* num_indices)
{
	*num_indices = 0;
	for (tinyobj::shape_t s : shapes)
		for (unsigned char c : s.mesh.num_face_vertices)
			*num_indices += c;
	GLuint* indices = new GLuint[(*num_indices)];

	size_t cur_ind = 0;
	for (tinyobj::shape_t s : shapes)
	{
		size_t offset = 0;
		for (size_t cface = 0; cface < s.mesh.num_face_vertices.size(); cface++)
		{
			size_t face_verts = s.mesh.num_face_vertices[cface];//number of vertices in current face

			for (size_t vert = 0; vert < face_verts; vert++)
			{
				tinyobj::index_t id = s.mesh.indices[offset + vert];
				indices[cur_ind] = id.vertex_index;
				cur_ind++;

			}
			offset += face_verts;
		}
	}
	*ind = indices;
}

void ObjectLoader::write_sphere(Sphere sp, std::string name)
{
	std::cout << "Writing the model..." << std::endl;

	//create file
	//write first part
	std::ofstream file;
	file.open(name);
	file << "# model.obj" << std::endl << "o europa" << std::endl << std::endl;

	//write verts
	for (int i = 0; i < sp.num_verts; i++)
	{
		file << "v " << sp.verts[i].x << " " << sp.verts[i].y << " " << sp.verts[i].z << std::endl;
	}

	//write normals
	for (int i = 0; i < sp.num_verts; i++)
	{
		file << "vn " << sp.normals[i].x << " " << sp.normals[i].y << " " << sp.normals[i].z << std::endl;
	}

	//write texcoords
	for (int i = 0; i < sp.num_verts; i++)
	{
		file << "vt " << sp.texture_coords[i].x << " " << sp.texture_coords[i].y << std::endl;
	}


	//done
	file.close();

	std::cout << "Finished writing the model!" << std::endl;
}

ObjectLoader::ObjectLoader()
{
}


ObjectLoader::~ObjectLoader()
{
}
