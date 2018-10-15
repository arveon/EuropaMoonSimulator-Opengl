#include "GLManager.h"



GLManager::GLManager()
{
	
}


GLManager::~GLManager()
{
}

void GLManager::init()
{
	if (!glfwInit())
	{
		std::cout << "Failed to init GLFW." << std::endl;
		exit(EXIT_FAILURE);
	}
	win = glfwCreateWindow(640, 480, "Simplest", NULL, NULL);
	glfwMakeContextCurrent(win);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	events.set_error_callback(error_callback);
	events.set_reshape_callback(win, resize_callback);
	events.set_key_callback(win, key_callback);

	init_objects();
}

void GLManager::init_objects()
{
	triangle = Triangle("../shaders/basic.vert", "../shaders/basic.frag");
	square = Square("../shaders/basic.vert", "../shaders/basic.frag");
}

void GLManager::loop()
{
	while (!glfwWindowShouldClose(win))
	{
		render();
		glfwSwapBuffers(win);
		glfwPollEvents();
	}
}

void GLManager::render()
{
	std::stack<glm::mat4> transf;
	transf.push(glm::mat4(1.0f));
	
	transf.top() = glm::scale(transf.top(),glm::vec3(.5f, .5f, 1.f));
	transf.top() = glm::rotate(transf.top(), glm::radians(30.f), glm::vec3(0, 0, 1));

	//move square to the left
	transf.push(transf.top());
	{
		transf.top() = glm::translate(transf.top(), glm::vec3(-1.f, 0.f, 0.f));
		square.set_model_matrix(transf.top());
	}
	transf.pop();
	
	//move square to the right
	transf.top() = glm::translate(transf.top(), glm::vec3(1.f, 0.f, 0.f));
	triangle.set_model_matrix(transf.top());

	
	square.draw();
	triangle.draw();
}

void GLManager::terminate()
{
	glfwTerminate();
}

void GLManager::resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void GLManager::error_callback(int error, const char* desc)
{
	fputs(desc, stderr);
}

void GLManager::key_callback(GLFWwindow* window, int key_code, int scancode, int action, int mods)
{

}


