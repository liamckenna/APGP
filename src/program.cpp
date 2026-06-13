#include "program.h"
#include "json.h"
#include <iostream>
#include <thread>
#include <GL/glew.h>
#include "callbacks.h"
#include "scene.h"
#include "clock.h"
#include "windows.h"
#include "graphics_config.h"
#include "universal_vars.h"
#include "precise_timer.h"


Program::Program(const std::string& filepath) : clock(), input_manager(*this) {

	nlohmann::json data = ReadJsonFromFile(filepath);

	hardcoded = Fetch(data, "hardcoded", false);
	limit_fps = Fetch(data, "limit_fps", false);
	use_compute = Fetch(data, "use_compute", true);
	fps_cap = Fetch(data, "fps_cap", 60);
	target_frame_time = 1.0 / static_cast<double>(fps_cap);

	glfwInit();

	std::string graphics_filepath = "/data/jsons/graphics/" + std::string(data["graphics_config"]);
	graphics_config.emplace(graphics_filepath);
	std::cout << "configured graphics" << std::endl;
	
	std::string windows_filepath = "/data/jsons/windows/" + std::string(data["windows"]);
	windows = new Windows(windows_filepath, *this);
	graphics_config.value().ApplyOpenGLSettings(); //needs to happen after the window is made
	std::cout << "initialized windows" << std::endl;

	glewInit();

	std::string shaders_filepath = "/data/jsons/shaders/" + std::string(data["shaders"]);
	shader_manager.LoadFromJSON(shaders_filepath);
	std::cout << "initialized shaders and their uniforms" << std::endl;

	SetCallbacks(windows->program_window);

	scene = nullptr;
	
	std::cout << "initialized program" << std::endl;
}

void Program::Run() {

	const auto period = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
		std::chrono::duration<double>(target_frame_time));
	auto next_present = std::chrono::steady_clock::now() + period;

	do {
		clock.Tick();

		glfwPollEvents();

		scene->scene_ecs.Update(clock.GetDeltaTime());

		glfwSwapBuffers(windows->program_window->glfw_window);

		if (limit_fps && graphics_config->swap_interval == 0) {
			precise_timer.WaitUntil(next_present);
			next_present += period;

			auto now = std::chrono::steady_clock::now();
			if (now - next_present > period)
				next_present = now + period;
		}

	} while (!glfwWindowShouldClose(windows->program_window->glfw_window));

}

void Program::Cleanup() {
	//cleanup buffers and shaders
	std::cout << "Program Average FPS: " << clock.GetFinalFPS() << std::endl;
	glfwDestroyWindow(windows->program_window->glfw_window);
	glfwTerminate();

}