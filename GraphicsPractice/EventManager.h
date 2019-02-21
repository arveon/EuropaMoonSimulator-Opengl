/* by Aleksejs Loginovs - October 2018 */

/*
Class contains callbacks for actions like cursor move, click etc.
*/
#pragma once
#include <GLFW/glfw3.h>
#include <GLFW/glfw3.h>

class EventManager
{
private:
	void(*error_callback) (int error, const char* description);
	void(*reshape)(GLFWwindow* window, int w, int h);
	void(*key_callback)(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
	void set_error_callback(void(*f)(int error, const char* desc))
	{
		this->error_callback = f;
		glfwSetErrorCallback(error_callback);
	}
	void set_reshape_callback(GLFWwindow* window, void(*f)(GLFWwindow* window, int w, int h))
	{
		this->reshape = f;
		glfwSetFramebufferSizeCallback(window, f);
	}
	void set_key_callback(GLFWwindow* window, void(*f)(GLFWwindow* window, int key, int scancode, int action, int mods))
	{
		this->key_callback = f;
		glfwSetKeyCallback(window, key_callback);
	}

	void set_cursor_callback(GLFWwindow* window, void(*f)(GLFWwindow* window, double xpos, double ypos))
	{
		glfwSetCursorPosCallback(window, f);
	}
	
	EventManager() {}
	~EventManager() {}
};

