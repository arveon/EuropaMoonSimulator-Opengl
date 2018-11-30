/*	by Aleksejs Loginovs - November 2018 
	Mostly adapted from Iain Martin's object loading example code
*/

#pragma once
#include "tiny_obj_loader.h"
#include <vector>

#include "Drawable.h"


class ObjectLoader
{
public:
	static Drawable* load_object(std::string obj_path);
	static void parse_indices(std::vector<tinyobj::shape_t> shapes, GLuint ** indices, int * num_indices);

	ObjectLoader();
	~ObjectLoader();
};