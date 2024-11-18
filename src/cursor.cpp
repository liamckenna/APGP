#include "cursor.h"

Cursor::Cursor(Window* window, float sensitivity) {
	last_x = window->width / 2.0f; //center of screen
	last_y = window->height / 2.0f; //^
	first = true;
	this->window = window;
	this->sensitivity = sensitivity;
}

void Cursor::Update() {

	glfwGetCursorPos(window->window, &curr_x, &curr_y);

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
