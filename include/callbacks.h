#pragma once
#include <GLFW/glfw3.h>

void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void WindowFocusCallback(GLFWwindow* window, int focused);

void SetCallbacks(GLFWwindow* window, void* user);