#pragma once
#include "util.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "json.h"

enum DISPLAY_MODE {
	WINDOWED = 0b01,
	FULLSCREEN = 0b10,
	WINDOWED_FULLSCREEN = 0b11
};

struct Cursor;
struct Program;
struct Window {
	GLFWwindow* glfw_window;
	Cursor& cursor;
	
	Program& program;

	int idx;

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
	GLenum cursor_mode;
	int swap_interval;

	Window(const nlohmann::json& settings, Program& program);
	Window(uint width, uint height, uint msaa, int pos_x, int pos_y, bool resizable, bool decorated, bool focused, bool visible, DISPLAY_MODE display_mode, GLenum cursor_mode, std::string title, uint id);

	void SetGLFWwindowHints();
	void SetInputMode();
	void SetSwapInterval();
	void BindGLFWwindow();
};