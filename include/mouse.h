#pragma once
#include <GLFW/glfw3.h>

struct Mouse {
	float last_x;
	float last_y;

	double curr_x;
	double curr_y;

	float offset_x;
	float offset_y;

	float sensitivity;

	GLFWwindow* window;

	bool first;

	Mouse(float screen_width, float screen_height, GLFWwindow* window, float sensitivity = 3.f);
	
	void Update();
};
