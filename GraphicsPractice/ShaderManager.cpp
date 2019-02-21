/* by Aleksejs Loginovs - October 2018 */

#include "ShaderManager.h"

/*
Function used to run through the whole process of loading a set of shaders for a pipeline
*/
Shader ShaderManager::load_shader(const char* vertex_shader_path, const char* fragment_shader_path, const char* geometry_shader_path)
{
	GLuint v_shader, f_shader, g_shader;

	std::string v_shader_txt = read_file(vertex_shader_path);
	std::string f_shader_txt = read_file(fragment_shader_path);

	v_shader = build_shader(GL_VERTEX_SHADER, v_shader_txt);
	f_shader = build_shader(GL_FRAGMENT_SHADER, f_shader_txt);
	if (geometry_shader_path != NULL)
	{
		std::string g_shader_txt = read_file(geometry_shader_path);
		g_shader = build_shader(GL_GEOMETRY_SHADER, g_shader_txt);
	}

	GLuint program = glCreateProgram();

	glAttachShader(program, v_shader);
	if (geometry_shader_path != NULL)
		glAttachShader(program, g_shader);
	glAttachShader(program, f_shader);
	glLinkProgram(program);

	GLint res = GL_FALSE;
	int log_length;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
	char* program_error = (char*)malloc(sizeof(char) * (log_length + 1));
	glGetProgramInfoLog(program, log_length, NULL, program_error);
	if(log_length > 0)
		std::cout << program_error << std::endl;

	glDeleteShader(v_shader);
	glDeleteShader(f_shader);
	if(geometry_shader_path != NULL)
		glDeleteShader(g_shader);

	return Shader(program);
}

/*
Function used to build a shader
*/
GLuint ShaderManager::build_shader(GLenum shaderType, const std::string & shaderText)
{
	GLuint shader = glCreateShader(shaderType);

	const char* strShaderText = shaderText.c_str();
	glShaderSource(shader, 1, &strShaderText, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* logString = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, logString);

		const char* shader_type_string = NULL;
		switch (shaderType)
		{
		case GL_VERTEX_SHADER:
			shader_type_string = "vertex";
			break;
		case GL_FRAGMENT_SHADER:
			shader_type_string = "fragment";
			break;
		case GL_GEOMETRY_SHADER:
			shader_type_string = "geometry";
			break;
		}
		std::cerr << "Compile error in " << shader_type_string << " shader.\n\t" << logString << std::endl;
		delete[] logString;
		throw std::exception("shader compile exception");
	}

	return shader;
}

/*
Function used to read file into a string
*/
std::string ShaderManager::read_file(const char * file_path)
{
	std::string content;
	std::ifstream file_stream(file_path, std::ios::in);

	if (!file_stream.is_open())
	{
		std::cerr << "Couldn't read file: " << file_path << std::endl;
		return "";
	}

	std::string line = "";
	while (!file_stream.eof())
	{
		std::getline(file_stream, line);
		content.append(line + "\n");
	}
	file_stream.close();

	return content;
}

ShaderManager::ShaderManager()
{
}


ShaderManager::~ShaderManager()
{
}
