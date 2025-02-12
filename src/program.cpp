#include "program.h"
#include "json.h"
#include <iostream>
#include "user.h"
#include "glfw_setup.h"
#include "glew_init.h"
#include "callbacks.h"
#include "scene.h"
#include "clock.h"
#include "windows.h"
#include "graphics_config.h"

Program::Program(const std::string& filepath) {

	nlohmann::json program_json = ReadJsonFromFile(filepath);

	InitializeGLFW();

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

	InitializeGLEW();

	std::string shaders_filepath = "/data/jsons/shaders/" + std::string(program_json["shaders"]);
	shaders = new Shaders(shaders_filepath, this);
	std::cout << "initialized shaders" << std::endl;

	SetCallbacks(windows->program_window);

	std::cout << "initialized program" << std::endl;
}

void Program::Run() {
	//move loop here
}