#include "windows.h"
#include <iostream>
#include "json.h"

Windows::Windows(const std::string& filepath, Program& program) : program(program) {
    it = 0;
    nlohmann::json data = ReadJsonFromFile(filepath);
    Initialize(data);
}

Windows::~Windows() {
    for (auto& [name, window] : windows) {
        delete window;
    }
    windows.clear();
}

bool Windows::Initialize(const nlohmann::json& settings) {
    if (!settings.contains("windows") || !settings["windows"].is_array()) {
        std::cerr << "No valid window configuration found!" << std::endl;
        return false;
    }


    for (int i = 0; i < settings["windows"].size(); i++) {
        Window* newWindow = CreateWindow(settings["windows"][i]);
        if (!newWindow) {
            return false;
        }
    }

    SetProgramWindow(settings["program_window"]);
    SetActiveWindow(settings["active_window"]);

    program_window->SetSwapInterval();

    return true;
}

Window* Windows::CreateWindow(const nlohmann::json& settings) {
    Window* newWindow = new Window(settings, program);
    windows[it] = newWindow;
    it++;
    return newWindow;
}

void Windows::RemoveWindow(uint window_id) {
    if (windows.find(window_id) == windows.end()) {
        std::cerr << "Window with ID " << window_id << " does not exist!" << std::endl;
        return;
    }

    if (window_id == program_window->idx) {
        std::cerr << "Program window removed. Terminating application..." << std::endl;
        delete windows[window_id];
        windows.erase(window_id);
        glfwTerminate();
        exit(EXIT_SUCCESS);
    }

    delete windows[window_id];
    windows.erase(window_id);

    if (window_id == active_window->idx) {
        SetActiveWindow(program_window->idx);
    }

}

Window* Windows::GetWindow(uint window_id) {
    if (windows.find(window_id) != windows.end()) {
        return windows[window_id];
    }
    return nullptr;
}

void Windows::SetProgramWindow(uint window_id) {
    if (windows.find(window_id) == windows.end()) {
        std::cerr << "Cannot set main window: " << window_id << " does not exist!" << std::endl;
        return;
    }
    program_window = windows[window_id];
    glfwMakeContextCurrent(program_window->glfw_window);
}

void Windows::SetActiveWindow(uint window_id) {
    if (windows.find(window_id) == windows.end()) {
        std::cerr << "Window ID " << window_id << " does not exist!" << std::endl;
        return;
    }
    active_window = windows[window_id];
    active_window->cursor.current_window = active_window;
    glfwMakeContextCurrent(active_window->glfw_window);
}
