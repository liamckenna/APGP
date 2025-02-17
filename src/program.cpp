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

Program::Program(const std::string& filepath) {

	nlohmann::json program_json = ReadJsonFromFile(filepath);

	glfwInit();

	clock = new Clock();
	std::cout << "initialized clock" << std::endl;

	std::string graphics_filepath = "/data/jsons/graphics/" + std::string(program_json["graphics"]);
	graphics_config = new GraphicsConfig(graphics_filepath);
	std::cout << "configured graphics" << std::endl;

	std::string user_filepath = "/data/jsons/users/" + std::string(program_json["user"]);
	user = new User(user_filepath, this);
	std::cout << "initialized user" << std::endl;

	std::string windows_filepath = "/data/jsons/windows/" + std::string(program_json["windows"]);
	windows = new Windows(windows_filepath, this);
	std::cout << "initialized windows" << std::endl;

	glewInit();

	std::string shaders_filepath = "/data/jsons/shaders/" + std::string(program_json["shaders"]);
	shaders = new Shaders(shaders_filepath, this);
	std::cout << "initialized shaders and their uniforms" << std::endl;

	SetCallbacks(windows->program_window);

	std::cout << "initialized program" << std::endl;
}

void Program::Run() {
	
	do {
		// Calculate frame timing
		clock->Tick();

		//scene->Render();

		// Swap buffers and poll events
		glfwSwapBuffers(windows->program_window->glfw_window);
		//glfwPollEvents();

	} while (!glfwWindowShouldClose(windows->program_window->glfw_window));

}

void Program::Cleanup() {
	//cleanup buffers and shaders
	glfwDestroyWindow(windows->program_window->glfw_window);
	glfwTerminate();
}