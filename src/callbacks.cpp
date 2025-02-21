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
	user_window->cursor.dx *= 100.f / user_window->width;
	user_window->cursor.dy *= 100.f / user_window->height;

}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

	Window* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
	Program& program = user_window->program;
	Clock& clock = program.clock;
	Scene* scene = program.scene;
	
	//todo: need to standardize this and allow for custom code injection

	//Object* camera = scene->GetObjectByName("camera");

	//if (yoffset != 0 && camera->t->local.pos[2] >= 0) { //scrolling up
	//	if (glfwGetKey(scene->user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
	//		scene->GetLightByName("flashlight")->strength += (yoffset * 1000.f * clock->GetDeltaTime());
	//		std::cout << "strength: " << scene->GetLightByName("flashlight")->strength << std::endl;
	//	}
	//	else {
	//		camera->t->local.TranslateForward(dynamic_cast<Camera*>(camera->GetChildByNameTree("camera 1"))->velocity * yoffset * 500.f, clock->GetDeltaTime());
	//		camera->t->UpdateGlobal();
	//	}

	//}
	//if (camera->t->local.pos[2] < 0) {
	//	if (glfwGetKey(scene->user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
	//		scene->GetLightByName("flashlight")->strength -= (yoffset * 1000.f * clock->GetDeltaTime());
	//		std::cout << "strength: " << scene->GetLightByName("flashlight")->strength << std::endl;
	//	}
	//	else {
	//		camera->t->local.SetValue(camera->t->local.pos, glm::vec3(camera->t->local.pos[0], camera->t->local.pos[1], 0));
	//		camera->t->UpdateGlobal();
	//	}

	//}

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