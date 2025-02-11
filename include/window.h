#pragma once
#include "defs.h"

#include <GLFW/glfw3.h>
#include <string>
#include "json.h"
enum DISPLAY_MODE {
	WINDOWED = 0b01,
	FULLSCREEN = 0b10,
	WINDOWED_FULLSCREEN = 0b11
};

struct Cursor;

struct Window {
	GLFWwindow* glfw_window;
	Cursor* cursor;

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
	

	Window(const nlohmann::json& settings);
	Window(uint width, uint height, uint msaa, int pos_x, int pos_y, bool resizable, bool decorated, bool focused, bool visible, std::string display_mode, std::string title);

	void SetGLFWwindowHints();
	void BindGLFWwindow();
};