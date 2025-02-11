#pragma once

struct Window;

struct Cursor {
	float last_x;
	float last_y;

	float offset_x;
	float offset_y;

	float sensitivity;

	bool first;

	Window* window;

	Cursor(Window* window);
	
	void Update(double xpos, double ypos);
};
