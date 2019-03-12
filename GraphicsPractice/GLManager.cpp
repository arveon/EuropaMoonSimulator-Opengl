/* by Aleksejs Loginovs - October 2018 */

#include "GLManager.h"

glm::vec3 GLManager::light_movement;

GLfloat GLManager::aspect_ratio;

float GLManager::unaffected_time = 0;
int GLManager::speed = 300;

CameraController GLManager::camera;

bool GLManager::reset = false;
bool GLManager::close = false;
bool GLManager::show_cursor = false;
bool GLManager::attenuation_enabled = true;
bool GLManager::texture_enabled = true;
bool GLManager::light_enabled = true;
bool GLManager::colour_enabled = false;
bool GLManager::draw_normals = false;
bool GLManager::exporting = false;
int GLManager::sphere_mode = 0;

const float GLManager::planet_rotation_speed = 0.02f;

glm::vec3 GLManager::cursor_movement;

///Function used to reset the scene (camera,  sun, time speed)
void GLManager::reset_scene()
{
	sun.move_to(glm::vec4(0, 0, 0, 1));
	camera.reset_camera();
	reset = false;
	speed = 300;
}

GLManager::GLManager()
{
	srand(time(0));
}


GLManager::~GLManager()
{
}

///Function used to initialise all of the required opengl components and load the required textures
void GLManager::init()
{
	if (!glfwInit())
	{
		std::cout << "Failed to init GLFW." << std::endl;
		exit(EXIT_FAILURE);
	}

	//set up window
	win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Simplest", NULL, NULL);
	aspect_ratio = WINDOW_WIDTH / WINDOW_HEIGHT;
	glfwMakeContextCurrent(win);
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//make sure glew is initialised
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	//register all event callbacks
	events.set_error_callback(error_callback);
	events.set_reshape_callback(win, resize_callback);
	events.set_key_callback(win, key_callback);
	events.set_cursor_callback(win, cursor_moved_callback);
	glfwSetScrollCallback(win, scroll_callback);

	//load required shaders
	try
	{
		basic_shader = ShaderManager::load_shader("../shaders/lit_textured.vert", "../shaders/lit_textured.frag");
		basic_shader.init_shader(aspect_ratio, LIT_TEXTURED_SHADER);
		basic_shader.set_shininess(1);

		lightsource_shader = ShaderManager::load_shader("../shaders/lightsource.vert", "../shaders/lightsource.frag");
		lightsource_shader.init_shader(aspect_ratio, BASIC_SHADER);

		unlit_texture_shader = ShaderManager::load_shader("../shaders/unlit_textured.vert", "../shaders/unlit_textured.frag");
		unlit_texture_shader.init_shader(aspect_ratio, BASIC_SHADER);

		normals_shader = ShaderManager::load_shader("../shaders/normals.vert", "../shaders/normals.frag", "../shaders/normals.geom");
		normals_shader.init_shader(aspect_ratio, BASIC_SHADER);

		particle_shader = ShaderManager::load_shader("../shaders/particle.vert", "../shaders/particle.frag", "../shaders/particle.geom");
		particle_shader.init_shader(aspect_ratio, BASIC_SHADER);
	}
	catch (std::exception e)
	{
		std::cout << "Couldn't load shader: " << e.what() << std::endl;
		std::cin.ignore();
		exit(EXIT_FAILURE);
	}

	//load required textures
	try
	{
		terrain_tex = SOIL_load_OGL_texture("..\\textures\\ice.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

		if (terrain_tex == 0)
			std::cerr << "Error loading texture: " << SOIL_last_result() << std::endl;

		int loc = glGetUniformLocation(basic_shader.get_program_id(), "tex");
		if (loc >= 0) glUniform1i(loc, 0);
	}
	catch (std::exception)
	{
		std::cerr << "Couldn't load a texture." << std::endl;
	}

	//enable depth texting
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.4f, .4f, .6f, 1);

	// Enable face culling. This will cull the back faces of all
	// triangles. Be careful to ensure that triangles are drawn
	// with correct winding.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	init_objects();
}

///Function used to separate the object initialisation logic out
void GLManager::init_objects()
{
	camera.set_mode(CameraController::controlled);

	UserPrefs prefs = Assistant::engage();

	if (prefs.import)
	{
		sphere = ObjectLoader::load_object_sphere(prefs.src_file, basic_shader);
		sphere->set_shader(basic_shader);
		sphere->set_texture(terrain_tex);
		sphere->reload_in_memory();
		model_exported = true;
	}
	else if (!prefs.import)
	{
		sphere = terrain_gen.create_terrain_on_sphere(basic_shader, 1000, 1000, prefs, terrain_tex);
		model_exported = false;
	}

	//initialise the sphere position
	sphere->set_normal_shader(normals_shader);
	sphere->normals_enabled = false;

	//initialise shaders
	basic_shader.set_shininess(8.f);

	//initialise lightsource
	/*sun = Lightsource(lightsource_shader);
	sun.set_scale(glm::vec3( .3f, .3f, .3f));*/


	GLManager::print_controls();
}

void GLManager::loop()
{
	glfwSetTime(0);
	while (!glfwWindowShouldClose(win))
	{
		//calculate time from last tick
		static float prev_time = 0;
		unaffected_time = (glfwGetTime() - prev_time);
		float delta_time = unaffected_time * speed;
		prev_time = glfwGetTime();

		update(delta_time);
		render();
		glfwSwapBuffers(win);
		glfwPollEvents();

		if (close == true)
			glfwSetWindowShouldClose(GLManager::win, GL_TRUE);
	}
}

void GLManager::update(float delta_time)
{
	camera.update(unaffected_time, cursor_movement);
	//cursor_movement = glm::vec2(0);//reset cursor delta movement every tick
	cursor_movement.z = 0;

	//set projection matrix in all shaders
	glm::mat4 projection = glm::perspective(glm::radians(60.f), aspect_ratio, 0.1f, 200.f);
	basic_shader.set_projection_matrix(projection);
	lightsource_shader.set_projection_matrix(projection);
	particle_shader.set_projection_matrix(projection);
	unlit_texture_shader.set_projection_matrix(projection);
	normals_shader.set_projection_matrix(projection);

	//update sphere view matrix and position
	sphere->set_view_matrix(camera.get_view_matrix());
	sphere->set_draw_normals(draw_normals);
	sphere->scale(glm::vec3(10.f, 10.f, 10.f));
	
	int state = glfwGetMouseButton(this->win, GLFW_MOUSE_BUTTON_LEFT);
	static float d = 0;
	if (state == GLFW_RELEASE)
		d += (d >= 360) ? int(std::round(d)) % 360 : planet_rotation_speed * delta_time;
	sphere->rotate(glm::radians(d), glm::vec3(0, 1, 0));
	sphere->rotate(glm::radians(cursor_movement.x), glm::vec3(0, 1, 0));

	//update the lightsource position
	//sun.move_to(glm::vec4(campos,1));

	//set the light position in shader
	basic_shader.set_light_position(camera.get_view_matrix()*sun.get_position());


	//apply scene changes if specific flags were set
	if (reset)
		reset_scene();
	basic_shader.set_attenuation_enabled(attenuation_enabled);
	basic_shader.set_texture_enabled(texture_enabled);
	basic_shader.set_colour_enabled(colour_enabled);
	basic_shader.set_lighting_enabled(light_enabled);

	if (exporting && !model_exported)
	{
		ObjectLoader::write_sphere(*sphere);
		exporting = false;
	}
	else if (exporting && model_exported)
		std::cout << "Model already exported" << std::endl;

}

void GLManager::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//sun.draw();
	sphere->drawSphere(sphere_mode);
}

void GLManager::terminate()
{
	glfwTerminate();
}

void GLManager::resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	aspect_ratio = float(width) / float(height);
}

void GLManager::error_callback(int error, const char* desc)
{
	fputs(desc, stderr);
}

void GLManager::key_callback(GLFWwindow* window, int key_code, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		//light movement
		if (key_code == GLFW_KEY_UP)
			light_movement.z = -LIGHT_MOVEMENT_SPEED;
		else if (key_code == GLFW_KEY_DOWN)
			light_movement.z = LIGHT_MOVEMENT_SPEED;
		if (key_code == GLFW_KEY_RIGHT)
			light_movement.x = LIGHT_MOVEMENT_SPEED;
		else if (key_code == GLFW_KEY_LEFT)
			light_movement.x = -LIGHT_MOVEMENT_SPEED;
		if (key_code == GLFW_KEY_KP_ADD)
			light_movement.y = LIGHT_MOVEMENT_SPEED;
		else if (key_code == GLFW_KEY_KP_SUBTRACT)
			light_movement.y = -LIGHT_MOVEMENT_SPEED;

		//reset
		if (key_code == GLFW_KEY_R)
			reset = true;

		//close window
		if (key_code == GLFW_KEY_ESCAPE)
			close = true;

		//toggle colour
		if (key_code == GLFW_KEY_C)
		{
			colour_enabled = !colour_enabled;
			std::cerr << "color: " << colour_enabled << std::endl;
		}

		//toggle light
		if (key_code == GLFW_KEY_L)
		{
			light_enabled = !light_enabled;
			std::cerr << "light: " << light_enabled << std::endl;
		}

		//toggle normals
		if (key_code == GLFW_KEY_N)
		{
			draw_normals = !draw_normals;
			std::cout << "normals_enabled=" << draw_normals << std::endl;
		}
		//scroll through draw modes
		if (key_code == GLFW_KEY_M)
			sphere_mode = (sphere_mode < 2) ? ++sphere_mode : 0;


		//toggle attenuation
		if (key_code == GLFW_KEY_X)
			attenuation_enabled = !attenuation_enabled;

		//toggle texture
		if (key_code == GLFW_KEY_T)
			texture_enabled = !texture_enabled;

		//export model
		if (key_code == GLFW_KEY_E)
			exporting = true;
	}
	else if (action == GLFW_RELEASE)
	{
		//reset variables
		if (key_code == GLFW_KEY_RIGHT || key_code == GLFW_KEY_LEFT)
			light_movement.x = 0;
		if (key_code == GLFW_KEY_UP || key_code == GLFW_KEY_DOWN)
			light_movement.z = 0;
		if (key_code == GLFW_KEY_KP_ADD || key_code == GLFW_KEY_KP_SUBTRACT)
			light_movement.y = 0;
	}
}

void GLManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cursor_movement.z += yoffset / 2;
}

void GLManager::cursor_moved_callback(GLFWwindow * window, double xpos, double ypos)
{
	static int prev_xpos = NULL;
	//static int prev_ypos = NULL;

	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		if (prev_xpos == NULL)
		{//safeguard against a jump at the first call, when these values aren't set yet (=> equal to 0)
			prev_xpos = xpos;
			//prev_ypos = ypos;
		}

		glm::vec2 temp_delta;
		temp_delta.x = (floor(xpos) - prev_xpos);
		//temp_delta.y = (floor(ypos) - prev_ypos);

		cursor_movement.x += temp_delta.x / 4;
		//cursor_movement.y += temp_delta.y / 4;

		if (cursor_movement.x >= 360)
			cursor_movement.x = (int)cursor_movement.x % 360;
		else if (cursor_movement.x < 0)
			cursor_movement.x = 360 - (int)cursor_movement.x % 360;

		//if (cursor_movement.y >= 360)
		//	cursor_movement.y = (int)cursor_movement.y % 360;
		//else if (cursor_movement.y < 0)
		//	cursor_movement.y = 360 - (int)cursor_movement.y % 360;
	}

	prev_xpos = floor(xpos);
	//prev_ypos = floor(ypos);

}

void GLManager::print_controls()
{
}


