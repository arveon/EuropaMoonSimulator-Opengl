/*	by Aleksejs Loginovs - November 2018 
	Mostly adapted from Iain Martin's object loading example code
*/

#pragma once
#include "tiny_obj_loader.h"
#include <vector>
#include <fstream>
#include <iostream>

#include "Drawable.h"
#include "Sphere.h"
#include "Shader.h"


class ObjectLoader
{
public:
	static void write_sphere(Sphere sp);
	static Drawable* load_object(std::string obj_path);
	static Sphere* load_object_sphere(std::string obj_path, Shader shader);
	static void parse_indices(std::vector<tinyobj::shape_t> shapes, GLuint ** indices, int * num_indices);

	ObjectLoader();
	~ObjectLoader();
};