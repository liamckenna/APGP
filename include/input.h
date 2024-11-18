#pragma once
#include "cursor.h"
struct Input {

	Cursor* cursor;

	Input(Window* window, float mouse_sensitivity);
};