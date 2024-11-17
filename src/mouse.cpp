#include "mouse.h"

Mouse::Mouse(float screen_width, float screen_height, GLFWwindow* window, float sensitivity) {
	last_x = screen_width / 2.0f; //center of screen
	last_y = screen_height / 2.0f; //^
	first = true;
	this->window = window;
	this->sensitivity = sensitivity;
}

void Mouse::Update() {
	glfwGetCursorPos(window, &curr_x, &curr_y);
	if (first) {
		last_x = curr_x;
		last_y = curr_y;
		first = false;
	}

	offset_x = curr_x - last_x;
	offset_y = last_y - curr_y;

	last_x = curr_x;
	last_y = curr_y;
}
