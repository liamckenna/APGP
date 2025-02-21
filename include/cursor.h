#pragma once
struct Window;
struct Cursor {
	float x = 0;
	float y = 0;

	float dx = 0;
	float dy = 0;

	float sensitivity = 1;

	Window* current_window;

	Cursor() = default;
	
	void Update(double nx, double ny);
};
