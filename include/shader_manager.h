#pragma once

#include <GL/glew.h>
#include <unordered_map>
#include <string>
#include "json.h"
#include "uniforms.h"

class ShaderManager {
    std::unordered_map<std::string, GLuint> shader_programs;
    std::unordered_map<GLuint, Uniforms> shader_uniforms;
    GLuint active_shader = 0;

public:
    ShaderManager() = default;
    ~ShaderManager();

    void LoadFromJSON(const std::string& filepath);
    bool UseShader(const std::string& name);
    bool UseShader(GLuint program);
    GLuint GetShaderID(const std::string& name);
    GLuint GetActiveShader();

    void CacheUniforms(const std::string& shaderName);
    bool SetUniform(const std::string& shaderName, const std::string& uniformName, DataType value);
    bool SetUniform(const std::string& uniformName, DataType value);

    std::string filepath;
private:
    GLuint CompileShaderProgram(const std::string& vertexPath, const std::string& fragmentPath,
        const std::string& geometryPath = "",
        const std::string& tessControlPath = "",
        const std::string& tessEvalPath = "",
        const std::string& computePath = "");
};
