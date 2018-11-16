#include "ObjectLoader.h"



Drawable * ObjectLoader::load_object(std::string obj_path)
{
	Drawable* res = new Drawable();
	tinyobj::attrib_t obj;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	int num_verts, num_ind, num_normals, num_tex_coords = 0;

	GLfloat *vertices, *normals, *colors, *texcoords;
	vertices = normals = colors = texcoords = nullptr;
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
	vertices = new GLfloat[obj.vertices.size()];
	std::copy(obj.vertices.begin(), obj.vertices.end(), vertices);

	//copy stuff if present
	if (num_normals <= 0)
		std::cout << "No normals." << std::endl;
	else
	{
		normals = new GLfloat[obj.normals.size()];
		std::copy(obj.normals.begin(), obj.normals.end(), normals);
		res->normals_enabled = true;
	}

	if (num_tex_coords<= 0)
		std::cout << "No tex coords." << std::endl;
	else
	{
		texcoords = new GLfloat[obj.texcoords.size()];
		std::copy(obj.texcoords.begin(), obj.texcoords.end(), texcoords);
		res->tex_enabled = true;
	}

	colors = new GLfloat[obj.colors.size()];
	std::copy(obj.colors.begin(), obj.colors.end(), colors);


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
