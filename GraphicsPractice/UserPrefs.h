/* Class stores user choices in planet generator configuration */
#pragma once
#include <string>

class UserPrefs 
{
public:
	float ridge_density;
	float crater_density;
	float dome_density;

	bool import;
	std::string src_file;
};