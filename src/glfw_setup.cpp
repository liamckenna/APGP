#include "glfw_setup.h"
#include <iostream>
#include "json.h"
void InitializeGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    std::cout << "GLFW initialized" << std::endl;
}

void ConfigureGraphicsPipeline(const std::string& filepath, GLFWwindow* glfw_window) {
    nlohmann::json settings = ReadJsonFromFile(filepath);

	if (settings.contains("gl")) ConfigureOpenGL(settings["gl"]);
	if (settings.contains("glfw")) ConfigureGLFW(settings["glfw"], glfw_window);

}

void ConfigureOpenGL(const nlohmann::json& settings) {
	if (settings.contains("profile")) {
		if (settings["profile"] == "any") glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
		else if (settings["profile"] == "core") glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		else if (settings["profile"] == "compat") glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	}
	if (settings.contains("depth_test") && settings["depth_test"]) glEnable(GL_DEPTH_TEST);
	if (settings.contains("depth_function")) {
		std::string depth_function = std::string(settings["depth_function"]);
		if (depth_function == "false") glDepthFunc(GL_NEVER);
		else if (depth_function == "<") glDepthFunc(GL_LESS);
		else if (depth_function == "==") glDepthFunc(GL_EQUAL);
		else if (depth_function == "<=") glDepthFunc(GL_LEQUAL);
		else if (depth_function == ">") glDepthFunc(GL_GREATER);
		else if (depth_function == "!=") glDepthFunc(GL_NOTEQUAL);
		else if (depth_function == ">=") glDepthFunc(GL_GEQUAL);
		else if (depth_function == "true") glDepthFunc(GL_ALWAYS);
	}
	if (settings.contains("depth_mask")) glDepthMask(settings["depth_mask"] ? GL_TRUE : GL_FALSE);
	if (settings.contains("cull_face") && settings["cull_face"]) glEnable(GL_CULL_FACE);
	if (settings.contains("cull_side")) {
		if (settings["cull_side"] == "front") glCullFace(GL_FRONT);
		else if (settings["cull_side"] == "back") glCullFace(GL_BACK);
		else if (settings["cull_side"] == "front and back") glCullFace(GL_FRONT_AND_BACK);
	}
	if (settings.contains("front_face")) {
		if (settings["front_face"] == "cw") glFrontFace(GL_CW);
		else if (settings["front_face"] == "ccw") glFrontFace(GL_CCW);
	}
	if (settings.contains("line_width")) glLineWidth(settings["line_width"]);
	if (settings.contains("blend") && settings["blend"]) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

}

void ConfigureGLFW(const nlohmann::json& settings, GLFWwindow* glfw_window) {
	if (settings.contains("version_major")) glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings["version_major"]);
	if (settings.contains("version_minor")) glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings["version_minor"]);
	if (settings.contains("swap_interval")) glfwSwapInterval(settings["swap_interval"]);
	if (settings.contains("cursor_mode")) {
		if (settings["cursor_mode"] == "normal") glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else if (settings["cursor_mode"] == "hidden") glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		else if (settings["cursor_mode"] == "disabled") glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

}