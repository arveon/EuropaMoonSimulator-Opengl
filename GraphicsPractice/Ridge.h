#pragma once
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Ridge
{
private:
	//glm::vec3* verts;

public:
	//resolution can be anything
	//other values should be in range of 0-1
	static glm::vec3* generate_ridge(glm::vec2 resolution, float peak_height, float mid_low, float ridge_width);

	static glm::vec3* generate_crater(glm::vec2 resolution, float height, float radius);

	Ridge();
	~Ridge();
};

