#include "program.h"
#include "json.h"
#include <iostream>
#include "user.h"
#include "glfw_setup.h"
#include "glew_init.h"
#include "callbacks.h"
#include "scene.h"
Program::Program(const std::string& filepath) {

	InitializeGLFW();

	nlohmann::json program_json = ReadJsonFromFile(filepath);
	std::string user_filepath = "/data/jsons/users/" + std::string(program_json["user"]);
	
	user = new User(user_filepath, this);

	std::cout << "initialized user" << std::endl;

	std::string graphics_filepath = "/data/jsons/graphics/" + std::string(program_json["graphics"]);

	ConfigureGraphicsPipeline(graphics_filepath, user->window->glfw_window);

	SetCallbacks(user->window->glfw_window, user);

	InitializeGLEW();

	std::string scene_filepath = "/data/jsons/scenes/" + std::string(program_json["scene"]);

	scene = new Scene(scene_filepath, this);
	
	scene->user = user;

	std::cout << "initialized scene" << std::endl;

	nlohmann::json scene_json = ReadJsonFromFile(scene_filepath);
	if (ShaderInitialization(scene, scene_json)) {
		std::cout << "yeah shaders are good" << std::endl;
	}

	std::cout << "initialized program" << std::endl;

}