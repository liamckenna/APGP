#pragma once
#include "window.h"
#include "program.h"
#include <unordered_map>
#include <vector>
#include <string>

struct Windows {
    std::unordered_map<uint, Window*> windows;  // Store multiple windows by name
    Window* program_window;  // The primary window
    Window* active_window;

    Program& program;
    int it;

    Windows(const std::string& filepath, Program& program);
    ~Windows();

    bool Initialize(const nlohmann::json& settings);
    Window* CreateWindow(const nlohmann::json& settings);
    void RemoveWindow(uint window_id);
    Window* GetWindow(uint window_id);
    void SetActiveWindow(uint window_id);
    void SetProgramWindow(uint window_id);
};