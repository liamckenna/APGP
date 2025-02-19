#include "window.h"
#include "cursor.h"
#include <iostream>
#include "callbacks.h"
#include "program.h"
#include "graphics_config.h"
Window::Window(const nlohmann::json& settings, Program* program) {
	this->program = program;
	this->width = (settings.contains("width")) ? int(settings["width"]) : 1280;
	this->height = (settings.contains("height")) ? int(settings["height"]) : 720;
	this->msaa = (settings.contains("msaa")) ? int(settings["msaa"]) : 0;
	this->pos_x = (settings.contains("pos_x")) ? int(settings["pos_x"]) : 0;
	this->pos_y = (settings.contains("pos_y")) ? int(settings["pos_y"]) : 0;
	this->resizable = (settings.contains("resizable")) ? bool(settings["resizable"]) : false;
	this->decorated = (settings.contains("decorated")) ? bool(settings["decorated"]) : true;
	this->focused = (settings.contains("focused")) ? bool(settings["focused"]) : true;
	this->visible = (settings.contains("visible")) ? bool(settings["visible"]) : true;
	this->display_mode = WINDOWED;
	if (settings.contains("display_mode")) {
		if (std::string(settings["display_mode"]) == "windowed") this->display_mode = WINDOWED;
		else if (std::string(settings["display_mode"]) == "fullscreen") this->display_mode = FULLSCREEN;
		else if (std::string(settings["display_mode"]) == "windowed_fullscreen") this->display_mode = WINDOWED_FULLSCREEN;
	}
	this->cursor_mode = GLFW_CURSOR_NORMAL;
	if (settings.contains("cursor_mode")) {
		if (std::string(settings["cursor_mode"]) == "normal") this->cursor_mode = GLFW_CURSOR_NORMAL;
		else if (std::string(settings["cursor_mode"]) == "hidden") this->cursor_mode = GLFW_CURSOR_HIDDEN;
		else if (std::string(settings["cursor_mode"]) == "disabled") this->cursor_mode = GLFW_CURSOR_DISABLED;
	}
	this->swap_interval = program->graphics_config->swap_interval;

	this->title = (settings.contains("title")) ? std::string(settings["title"]) : "Window Title";

	this->cursor = new Cursor(this);

	std::cout << "after cursor" << std::endl;

	SetGLFWwindowHints();

	BindGLFWwindow();
	
	SetInputMode();
	SetSwapInterval();
}

Window::Window(uint width, uint height, uint msaa, int pos_x, int pos_y, bool resizable, bool decorated, bool focused, bool visible, DISPLAY_MODE display_mode, GLenum cursor_mode, std::string title, uint id) {
	this->idx = id;
	this->width = width;
	this->height = height;
	this->msaa = msaa;
	this->pos_x = pos_x;
	this->pos_y = pos_y;
	this->resizable = resizable;
	this->decorated = decorated;
	this->focused = focused;
	this->visible = visible;
	this->display_mode = display_mode;
	this->cursor_mode = cursor_mode;
	this->title = title;

	this->cursor = new Cursor(this);

	SetGLFWwindowHints();

	BindGLFWwindow();

	SetInputMode();
	SetSwapInterval();
}

void Window::SetGLFWwindowHints() {
	glfwWindowHint(GLFW_SAMPLES, msaa);
	glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, decorated ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_FOCUSED, focused ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);
	
}

void Window::SetInputMode() {
	glfwSetInputMode(glfw_window, GLFW_CURSOR, cursor_mode);
}

void Window::SetSwapInterval() {
	glfwSwapInterval(swap_interval);
	std::cout << "swap interval set to " << swap_interval << std::endl;
}

void Window::BindGLFWwindow() {
	
	if (display_mode == FULLSCREEN) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		glfw_window = glfwCreateWindow(width, height, title.c_str(), monitor, NULL);
	}
	else if (display_mode == WINDOWED_FULLSCREEN) {
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfw_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
		glfwSetWindowPos(glfw_window, 0, 0);
	}
	else {
		glfw_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
		glfwSetWindowPos(glfw_window, pos_x, pos_y);
	}

	if (!glfw_window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}
