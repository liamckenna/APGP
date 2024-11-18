#pragma once
#include "defs.h"
#include <GLFW/glfw3.h>
#include <string>
enum DISPLAY_MODE {
	WINDOWED = 0b01,
	FULLSCREEN = 0b10,
	WINDOWED_FULLSCREEN = 0b11
};

struct Window {
	GLFWwindow* window;
	std::string title;
	uint width;
	uint height;
	uint msaa;
	int pos_x;
	int pos_y;
	bool resizable;
	bool decorated;
	bool focused;
	bool visible;
	DISPLAY_MODE display_mode;
	

	Window();
	Window(uint width, uint height, uint msaa, int pos_x, int pos_y, bool resizable, bool decorated, bool focused, bool visible, std::string display_mode, std::string title);
};