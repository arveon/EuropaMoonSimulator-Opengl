#include "ObjectLoader.h"



Drawable * ObjectLoader::load_object(std::string obj_path)
{
	Drawable* res = new Drawable();
	tinyobj::attrib_t obj;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	int num_verts, num_ind, num_normals, num_tex_coords = 0;

	glm::vec4 *colors = nullptr;
	glm::vec3 *vertices, *normals;
	glm::vec2 *texcoords = nullptr;
	vertices = normals= nullptr;
	GLuint *indices = nullptr;


	std::string error, warning;
	bool ret = tinyobj::LoadObj(&obj, &shapes, &materials, &warning, &error, obj_path.c_str());
	if (!error.empty()) 
		std::cerr << error << std::endl;
	if (!warning.empty())
		std::cerr << warning << std::endl;

	
	num_verts = obj.vertices.size() / 3;
	num_normals = obj.normals.size() / 3;
	num_tex_coords = obj.texcoords.size() / 3;

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
		std::cout << "No normals." << std::endl;
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

	if (num_tex_coords<= 0)
		std::cout << "No tex coords." << std::endl;
	else
	{
		texcoords = new glm::vec2[obj.texcoords.size()];
		int counter = 0;
		for (unsigned int i = 0; i < obj.normals.size(); i += 2)
		{
			texcoords[counter] = glm::vec2(obj.texcoords[i], obj.texcoords[i + 1]);
			counter++;
		}

		res->tex_enabled = true;
	}

	colors = new glm::vec4[obj.colors.size()];
	counter = 0;
	for (unsigned int i = 0; i < obj.colors.size(); i += 4)
	{
		colors[counter] = glm::vec4(obj.colors[i], obj.colors[i + 1], obj.colors[i + 2], obj.colors[i + 3]);
		counter++;
	}
	//std::copy(obj.colors.begin(), obj.colors.end(), colors);


	int num_indices;
	parse_indices(shapes, &indices, &num_indices);

	res->init(vertices, obj.vertices.size(), colors, indices, num_indices, normals, texcoords);
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
			int face_verts = s.mesh.num_face_vertices[cface];//number of vertices in current face

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

ObjectLoader::ObjectLoader()
{
}


ObjectLoader::~ObjectLoader()
{
}
