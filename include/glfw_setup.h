#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <json.h>
void InitializeGLFW();

void ConfigureGraphicsPipeline(const std::string& filepath, GLFWwindow* glfw_window);

void ConfigureOpenGL(const nlohmann::json& settings);

void ConfigureGLFW(const nlohmann::json& settings, GLFWwindow* glfw_window);