/* by Aleksejs Loginovs - November 2018 */
/*
Class used to generate specific features on a flat dem to be applied to terrains later
Contains method versions for features to be applied to flat dem and for features to be applied to a sphere

*/
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

	static std::vector<glm::vec3> generate_ridge_sphere_angle(glm::vec2 resolution, float peak_height, float mid_low, float ridge_width, float angle);
	static std::vector<glm::vec3> generate_ridge_sphere(glm::vec2 resolution, float peak_height, float mid_low, float ridge_width);

	FeatureGenerator();
	~FeatureGenerator();
};

