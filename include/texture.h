#pragma once
#include <string>
#include <GL/glew.h>

struct Texture {
    std::string name;
    std::string file_path;
    GLuint texture_id = 0; // OpenGL texture handle
    int width = 0;
    int height = 0;
    int channels = 0;
    bool is_loaded = false;

    Texture() = default;
    Texture(const std::string& file_path);

    bool Load();
    void Bind(int slot = 0) const;
    void Unbind() const;
    void Free();
};
