#include "callbacks.h"
#include "program.h"
#include "clock.h"
#include "windows.h"
#include "input_manager.h"
#include <iostream>

void ErrorCallback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Window* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS) {
		user_window->program.input_manager.UpdateKeyState(key, true);
	}
	else if (action == GLFW_RELEASE) {
		user_window->program.input_manager.UpdateKeyState(key, false);
	}
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	Window* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
	user_window->cursor.Update(xpos, ypos);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	Window* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
	user_window->program.input_manager.wheel.active = true;
	user_window->program.input_manager.wheel.dx = xoffset;
	user_window->program.input_manager.wheel.dy = yoffset;
}

void WindowFocusCallback(GLFWwindow* window, int focused)
{
	Window* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
	InputManager& input_manager = user_window->program.input_manager;

	if (focused) {
		input_manager.UpdateAllKeyStates(window);
	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Window* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS) {
		user_window->program.input_manager.UpdateKeyState(button, true);
	}
	else if (action == GLFW_RELEASE) {
		user_window->program.input_manager.UpdateKeyState(button, false);
	}
}

void SetCallbacks(Window* window) {
	glfwSetWindowUserPointer(window->glfw_window, window);
	glfwSetScrollCallback(window->glfw_window, ScrollCallback);
	glfwSetErrorCallback(ErrorCallback);
	glfwSetKeyCallback(window->glfw_window, KeyCallback);
	glfwSetCursorPosCallback(window->glfw_window, MouseCallback);
	glfwSetWindowFocusCallback(window->glfw_window, WindowFocusCallback);
	glfwSetMouseButtonCallback(window->glfw_window, MouseButtonCallback);
}