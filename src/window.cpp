#include "window.h"

Window::Window() {

}


Window::Window(uint width, uint height, uint msaa, int pos_x, int pos_y, bool resizable, bool decorated, bool focused, bool visible, std::string display_mode, std::string title) {
	this->width = width;
	this->height = height;
	this->msaa = msaa;
	this->pos_x = pos_x;
	this->pos_y = pos_y;
	this->resizable = resizable;
	this->decorated = decorated;
	this->focused = focused;
	this->visible = visible;
	if (display_mode == "windowed") this->display_mode = WINDOWED;
	else if (display_mode == "fullscreen") this->display_mode = FULLSCREEN;
	else if (display_mode == "windowed_fullscreen") this->display_mode = WINDOWED_FULLSCREEN;
	this->title = title;
}