#pragma once
#include "window.h"

struct Cursor {
	float last_x;
	float last_y;

	float offset_x;
	float offset_y;

	float sensitivity;

	Window* window;

	bool first;



	Cursor(Window* window, float sensitivity = 3.f);
	
	void Update(double xpos, double ypos);
};
