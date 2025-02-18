#include "program.h"
#include "json.h"
#include <iostream>
#include <GL/glew.h>
#include "user.h"
#include "callbacks.h"
#include "scene.h"
#include "clock.h"
#include "windows.h"
#include "graphics_config.h"

Program::Program(const std::string& filepath) : clock() {

	nlohmann::json data = ReadJsonFromFile(filepath);

	hardcoded = Fetch(data, "hardcoded", false);

	glfwInit();

	std::string graphics_filepath = "/data/jsons/graphics/" + std::string(data["graphics"]);
	graphics_config.emplace(graphics_filepath);
	std::cout << "configured graphics" << std::endl;

	std::string user_filepath = "/data/jsons/users/" + std::string(data["user"]);
	user.emplace(user_filepath, this);
	std::cout << "initialized user" << std::endl;

	std::string windows_filepath = "/data/jsons/windows/" + std::string(data["windows"]);
	windows = new Windows(windows_filepath, this);
	std::cout << "initialized windows" << std::endl;

	glewInit();

	std::string shaders_filepath = "/data/jsons/shaders/" + std::string(data["shaders"]);
	shaders = new Shaders(shaders_filepath, this);
	std::cout << "initialized shaders and their uniforms" << std::endl;

	SetCallbacks(windows->program_window);

	std::cout << "initialized program" << std::endl;
}

void Program::Run() {
	
	do {
		
		clock.Tick();

		//here we need to do the rendering for our scene entities with meshes

		
		glfwSwapBuffers(windows->program_window->glfw_window);
		glfwPollEvents();

	} while (!glfwWindowShouldClose(windows->program_window->glfw_window));

}

void Program::Cleanup() {
	//cleanup buffers and shaders
	glfwDestroyWindow(windows->program_window->glfw_window);
	glfwTerminate();
}