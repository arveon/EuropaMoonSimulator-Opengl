#pragma once
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

class FeatureGenerator
{
private:
	//glm::vec3* verts;

public:
	//resolution can be anything
	//other values should be in range of 0-1
	static std::vector<glm::vec3> generate_ridge(glm::vec2 resolution, float peak_height, float mid_low, float ridge_width);

	static std::vector<glm::vec3>  generate_crater(glm::vec2 resolution, float height, float radius);

	FeatureGenerator();
	~FeatureGenerator();
};

