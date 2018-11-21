/* by Aleksejs Loginovs - October 2018 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <malloc.h>

#include "Shader.h"

//Class responsible for loading shaders
class ShaderManager
{
public:

	static Shader load_shader(const char* shader_path, const char* fragment_shader_path, const char* geometry_shader_path=NULL);
	static GLuint build_shader(GLenum shaderType, const std::string &shaderText);
	static std::string read_file(const char* file_path);

	ShaderManager();
	~ShaderManager();
};

