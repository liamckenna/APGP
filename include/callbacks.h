#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "window.h"
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void WindowFocusCallback(GLFWwindow* window, int focused);

void SetCallbacks(Window* window);