/* by Aleksejs Loginovs - October 2018 */
/*
Class that triggers everything else in the program
Contains the loop, update and draw methods
Is responsible for loading resources and initialising objects
*/
#pragma once
#define GLEW_STATIC

#pragma comment(lib, "glfw3D.lib")
#pragma comment(lib, "glew32sd.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "soil.lib")

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SOIL.h>

#include <stack>
#include <ctime>

#include "EventManager.h"
#include "ShaderManager.h"

#include "ObjectLoader.h"

#include "Cube.h"
#include "Sphere.h"
#include "Lightsource.h"

#include "TerrainGenerator.h"
#include "Terrain.h"

#include "ParticleSystem.h"

#include "Assistant.h"
#include "UserPrefs.h"

#include "CameraController.h"

class GLManager
{
private:
	GLFWwindow * win;
	EventManager events;

	//shaders
	Shader basic_shader = NULL;
	Shader lightsource_shader = NULL;
	Shader unlit_texture_shader = NULL;
	Shader normals_shader = NULL;
	Shader particle_shader = NULL;

	//textures
	GLuint terrain_tex;
	GLuint skybox_tex;

	//objects in the scene
	Drawable* skybox;
	Cube* cube;
	Lightsource sun;
	Sphere* sphere;

	TerrainGenerator terrain_gen;

	//control variables
	bool model_exported;
	
	static const float planet_rotation_speed;
	static glm::vec3 cursor_movement;
	static CameraController camera;
	static GLfloat aspect_ratio;
	static glm::vec3 light_movement;
	static float unaffected_time;
	static float camera_z;
	static int sphere_mode;
	

	//controll toggles
	static bool reset;
	static bool show_cursor;
	static bool attenuation_enabled;
	static bool texture_enabled;
	static bool light_enabled;
	static bool colour_enabled;
	static bool draw_normals;
	static bool close;
	static bool exporting;
	
	static int speed;
	
	//TESTING
	/*unsigned int depthMapFBO;
	unsigned int depthMap;*/

	void reset_scene();
public:
	GLManager();
	~GLManager();

	void init();
	void loop();
	void update(float delta_time);
	void render();
	void terminate();

	static void resize_callback(GLFWwindow* window, int width, int height);
	static void error_callback(int error, const char* desc);
	static void key_callback(GLFWwindow* window, int key_code, int scancode, int action, int mods);
	static void cursor_moved_callback(GLFWwindow* window, double xpos, double ypos);
	static void print_controls();
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void init_objects();
};

