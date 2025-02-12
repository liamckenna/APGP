#include "shaders.h"
#include "program.h"
#include "json.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
//disclaimer: got heavy help from chat here when refactoring this code out of main

Shaders::Shaders(const std::string& filepath, Program* program) {
    this->program = program;
    main_program = glCreateProgram();
    nlohmann::json data = ReadJsonFromFile(filepath);
    Initialize(data);
}

Shaders::~Shaders() {
    glDeleteProgram(main_program);
}

bool Shaders::Initialize(const nlohmann::json& data) {

    std::vector<std::pair<std::string, GLenum>> shaderTypes = {
        {"vertex", GL_VERTEX_SHADER},
        {"tess_control", GL_TESS_CONTROL_SHADER},
        {"tess_eval", GL_TESS_EVALUATION_SHADER},
        {"geometry", GL_GEOMETRY_SHADER},
        {"fragment", GL_FRAGMENT_SHADER},
        {"compute", GL_COMPUTE_SHADER}
    };

    if (data.contains("active")) {
        for (const auto& [key, _] : shaderTypes) {
            activeShaders[key] = data["active"].contains(key) ? data["active"][key].get<int>() : -1;
        }
    }

    for (auto& [key, shaderType] : shaderTypes) {
        if (!data.contains(key)) continue;

        int shaderIndex = 0;
        for (const auto& shaderData : data[key]) {
            if (!shaderData.contains("file")) continue;

            std::string filePath = "/shaders/" + std::string(shaderData["file"]);
            GLuint shaderID = CompileAndAttachShader(filePath, shaderType, main_program);

            if (shaderID == 0) return false; // Shader compilation failed

            compiledShaders[key].push_back(shaderID); // Store compiled shaders

            // Attach only if it's the active shader for this type
            if (shaderIndex == activeShaders[key]) {
                glAttachShader(main_program, shaderID);
            }

            shaderIndex++;
        }
    }

    glLinkProgram(main_program);
    GLint success;
    char infoLog[512];
    glGetProgramiv(main_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(main_program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }

    CacheUniformLocations();
    std::cout << "Shaders initialized successfully!" << std::endl;
    return true;
}



GLuint Shaders::CompileAndAttachShader(const std::string& filePath, GLenum type, GLuint program) {
    std::string source = LoadShader(filePath);
    if (source.empty()) {
        std::cerr << "Shader source loading failed: " << filePath << std::endl;
        return 0;
    }

    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    glAttachShader(program, shader);
    glDeleteShader(shader);
    return shader;
}

std::string Shaders::LoadShader(const std::string& filepath) {
    std::string absolute_filepath = std::filesystem::current_path().string() + filepath;
    std::ifstream file(absolute_filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Shaders::SetActiveShader(const std::string& shaderType, int newIndex) {
    if (compiledShaders.find(shaderType) == compiledShaders.end()) {
        std::cerr << "Shader type not found: " << shaderType << std::endl;
        return;
    }

    if (newIndex < 0 || newIndex >= compiledShaders[shaderType].size()) {
        std::cerr << "Invalid shader index for type: " << shaderType << std::endl;
        return;
    }

    // Detach the current active shader if one exists
    if (activeShaders[shaderType] != -1) {
        glDetachShader(main_program, compiledShaders[shaderType][activeShaders[shaderType]]);
    }

    // Attach the new active shader
    glAttachShader(main_program, compiledShaders[shaderType][newIndex]);
    activeShaders[shaderType] = newIndex;

    // Relink the shader program
    glLinkProgram(main_program);
}

GLint Shaders::GetUniformLocation(const std::string& name) {
    if (uniform_locations.find(name) != uniform_locations.end()) {
        return uniform_locations[name];
    }

    // If the uniform is not cached, try retrieving it dynamically
    GLint location = glGetUniformLocation(main_program, name.c_str());

    if (location == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader program!" << std::endl;
    }
    else {
        uniform_locations[name] = location; // Cache it for future use
    }

    return location;
}


void Shaders::CacheUniformLocations() {
    GLint uniformCount;
    glGetProgramiv(main_program, GL_ACTIVE_UNIFORMS, &uniformCount);

    for (GLint i = 0; i < uniformCount; i++) {
        char name[256];
        GLsizei length;
        GLint size;
        GLenum type;
        glGetActiveUniform(main_program, i, sizeof(name), &length, &size, &type, name);

        GLint location = glGetUniformLocation(main_program, name);
        uniform_locations[name] = location;

        std::cout << "Cached uniform: " << name << " at location " << location << std::endl;
    }
}