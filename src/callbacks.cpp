#include "callbacks.h"
#include "user.h"
#include "program.h"
#include "clock.h"
#include "windows.h"
#include "input.h"
#include <iostream>

void ErrorCallback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Window* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS) {
		user_window->program->user.value().input.UpdateKeyState(key, true);
	}
	else if (action == GLFW_RELEASE) {
		user_window->program->user.value().input.UpdateKeyState(key, false);
	}
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	Window* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
	Program* program = user_window->program;
	Clock& clock = program->clock;
	Window* program_window = program->windows->program_window;
	Cursor* cursor = program_window->cursor;


	//todo: replace with a program or scene function that handles camera movement i think
	//in the meantime, we just use the hardcoded method

	cursor->Update(xpos, ypos);

	cursor->offset_x *= cursor->sensitivity * clock.GetDeltaTime() / program_window->width * 100000.f;
	cursor->offset_y *= cursor->sensitivity * clock.GetDeltaTime() / program_window->height * 100000.f;


	//scene->GetObjectByName("camera shell")->t->local.RotateYaw(user->input->cursor->offset_x);
	//scene->GetObjectByName("camera shell")->t->local.RotatePitch(user->input->cursor->offset_y);
	//scene->GetObjectByName("camera shell")->UpdateTree();
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

	User* user = static_cast<User*>(glfwGetWindowUserPointer(window));
	Program* program = user->program;
	Clock& clock = program->clock;
	Scene* scene = program->scene;
	
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
	Input& input = user_window->program->user.value().GetInput();

	if (focused) {
		input.UpdateAllKeyStates(window);
	}
}

void SetCallbacks(Window* window) {
	glfwSetWindowUserPointer(window->glfw_window, window);
	glfwSetScrollCallback(window->glfw_window, ScrollCallback);
	glfwSetErrorCallback(ErrorCallback);
	glfwSetKeyCallback(window->glfw_window, KeyCallback);
	glfwSetCursorPosCallback(window->glfw_window, MouseCallback);
	glfwSetWindowFocusCallback(window->glfw_window, WindowFocusCallback);
}