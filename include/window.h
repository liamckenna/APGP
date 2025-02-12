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

enum CURSOR_MODE {
	NORMAL = GLFW_CURSOR_NORMAL,
	HIDDEN = GLFW_CURSOR_HIDDEN,
	DISABLED = GLFW_CURSOR_DISABLED
};
struct Cursor;

struct Window {
	GLFWwindow* glfw_window;
	Cursor* cursor;
	
	uint id;

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
	CURSOR_MODE cursor_mode;
	

	Window(const nlohmann::json& settings, uint id);
	Window(uint width, uint height, uint msaa, int pos_x, int pos_y, bool resizable, bool decorated, bool focused, bool visible, DISPLAY_MODE display_mode, CURSOR_MODE cursor_mode, std::string title, uint id);

	void SetGLFWwindowHints();
	void BindGLFWwindow();
};