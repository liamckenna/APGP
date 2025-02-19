#pragma once
#include <unordered_map>
#include <string>
#include "GL/glew.h"
#include "uniform.h"

struct Uniforms {
    std::unordered_map<std::string, Uniform> uniforms;
    GLuint program;

    Uniforms() = default;
    Uniforms(GLuint program);

    Uniform& GetByName(const std::string& name);
    void FindAndUpdate(const std::string& name, DataType value);
};
