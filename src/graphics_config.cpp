#include "graphics_config.h"
#include "util.h"
#include "json.h"
#include <iostream>

GraphicsConfig::GraphicsConfig() {
    DefaultOpenGLConfig();
    DefaultGLFWConfig();
    ApplyGLFWSettings();
}

GraphicsConfig::GraphicsConfig(const std::string& filepath) {


    nlohmann::json settings = ReadJsonFromFile(filepath);

    if (settings.contains("gl")) OpenGLConfig(settings["gl"]);
    else DefaultOpenGLConfig();
    if (settings.contains("glfw")) GLFWConfig(settings["glfw"]);
    else DefaultGLFWConfig();

    ApplyGLFWSettings();
}

void GraphicsConfig::OpenGLConfig(const nlohmann::json& settings) {

    default_draw_mode = FetchGLenum(Fetch(settings, "default_draw_mode", "GL_TRIANGLES"));

    depth_test = Fetch(settings, "depth_test", true);
    depth_function = FetchGLenum(Fetch(settings, "depth_function", "<"));

    depth_mask = Fetch(settings, "depth_mask", true);
    cull_face = Fetch(settings, "cull_face", true);

    cull_side = FetchGLenum(Fetch(settings, "cull_side", "back"));

    front_face = FetchGLenum(Fetch(settings, "front_face", "cw"));

    line_width = Fetch(settings, "line_width", 1.f);
    blend = Fetch(settings, "blend", true);
    
    polygon_mode = FetchGLenum(Fetch(settings, "polygon_mode", "fill"));
    polygon_mode_side = FetchGLenum(Fetch(settings, "polygon_mode_side", "front and back"));
}

void GraphicsConfig::GLFWConfig(const nlohmann::json& settings) {
    profile = FetchGLFWenum(Fetch(settings, "profile", "core"));
    gl_version_major = Fetch(settings, "version_major", 3);
    gl_version_minor = Fetch(settings, "version_minor", 3);
    swap_interval = Fetch(settings, "swap_interval", 0);
}

void GraphicsConfig::DefaultOpenGLConfig() {
    default_draw_mode = GL_TRIANGLES;
    depth_test = true;
    depth_function = GL_LESS;
    depth_mask = true;
    cull_face = true;
    cull_side = GL_BACK;
    front_face = GL_CW;
    line_width = 1.f;
    blend = true;
    polygon_mode = GL_FILL;
    polygon_mode_side = GL_FRONT_AND_BACK;
}

void GraphicsConfig::DefaultGLFWConfig() {
    profile = GLFW_OPENGL_CORE_PROFILE;
    gl_version_major = 3;
    gl_version_minor = 3;
    swap_interval = 0;
}


void GraphicsConfig::ApplySettings() const {
    ApplyGLFWSettings();
    ApplyOpenGLSettings();
}

void GraphicsConfig::ApplyOpenGLSettings() const {
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    if (cull_face) {
        glEnable(GL_CULL_FACE);
        glCullFace(cull_side);
    }
    else {
        glDisable(GL_CULL_FACE);
    }
    glFrontFace(front_face);
    glLineWidth(line_width);

    if (blend) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else {
        glDisable(GL_BLEND);
    }
    if (depth_test) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(depth_function);
        glDepthMask(depth_mask ? GL_TRUE : GL_FALSE);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
    glPolygonMode(polygon_mode_side, polygon_mode);

}

void GraphicsConfig::ApplyGLFWSettings() const {
    glfwWindowHint(GLFW_OPENGL_PROFILE, profile);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor);
    if (depth_test) glfwWindowHint(GLFW_DEPTH_BITS, 24);
}

