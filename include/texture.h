#pragma once
#include <string>
#include <GL/glew.h>

struct Texture {
    std::string name;
    GLuint index = -1;
    GLuint id = 0;
    int width = 0;
    int height = 0;
    int channels = 0;
    bool is_loaded = false;

    Texture() = default;
};
