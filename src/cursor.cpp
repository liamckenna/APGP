#include "cursor.h"

Cursor::Cursor(Window* window, float sensitivity) {
	last_x = window->width / 2.0f; //center of screen
	last_y = window->height / 2.0f; //^
	first = true;
	this->window = window;
	this->sensitivity = sensitivity;
}

void Cursor::Update(double xpos, double ypos) {

	if (first) {
		last_x = xpos;
		last_y = ypos;
		first = false;
	}

	offset_x = xpos - last_x;
	offset_y = last_y - ypos;

	last_x = xpos;
	last_y = ypos;
}
