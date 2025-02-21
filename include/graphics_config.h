#pragma once
#include <GL/glew.h>
#include "json.h"

struct GraphicsConfig {

    //opengl
    GLenum default_draw_mode;
    bool depth_test;
    GLenum depth_function;
    bool depth_mask;
    bool cull_face;
    GLenum cull_side;
    GLenum front_face;
    float line_width;
    bool blend;
    GLenum polygon_mode;
    GLenum polygon_mode_side;
    
    //glfw
    int profile;
    int gl_version_major;
    int gl_version_minor;
    int swap_interval;

    GraphicsConfig();
    GraphicsConfig(const std::string& filepath);

    void OpenGLConfig(const nlohmann::json& settings);
    void GLFWConfig(const nlohmann::json& settings);
    void DefaultOpenGLConfig();
    void DefaultGLFWConfig();

    void ApplySettings() const;
    void ApplyOpenGLSettings() const;
    void ApplyGLFWSettings() const;

};