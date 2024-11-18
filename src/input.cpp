#include "input.h"
#include "cursor.h"
Input::Input(Window* window, float mouse_sensitivity) {
	this->cursor = new Cursor(window, mouse_sensitivity);
}