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

	std::string graphics_filepath = "/data/jsons/graphics/" + std::string(data["graphics_config"]);
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
	shader_manager.LoadFromJSON(shaders_filepath);
	std::cout << "initialized shaders and their uniforms" << std::endl;

	SetCallbacks(windows->program_window);

	std::cout << "initialized program" << std::endl;
}

void Program::Run() {
	
	do {
		clock.Tick();

		glfwPollEvents();

		//will move to input manager
		if (user.value().input.GetKeyState(GLFW_KEY_SPACE) == PRESSED) {
			
		} else if (user.value().input.GetKeyState(GLFW_KEY_SPACE) == RELEASED) {
			//std::cout << "Space bar is RELEASED!" << std::endl;
		}
		
		scene->scene_ecs.Update(clock.GetDeltaTime());
		
		user.value().input.UpdateKeyStack();

		glfwSwapBuffers(windows->program_window->glfw_window);

	} while (!glfwWindowShouldClose(windows->program_window->glfw_window));

}

void Program::Cleanup() {
	//cleanup buffers and shaders
	glfwDestroyWindow(windows->program_window->glfw_window);
	glfwTerminate();
}