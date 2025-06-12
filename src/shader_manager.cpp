#include "shader_manager.h"
#include <iostream>
#include <fstream>

ShaderManager::~ShaderManager() {
    for (auto& [name, shader] : shader_programs) {
        glDeleteProgram(shader);
    }
}

int max_group_count_x = 65536;


void ShaderManager::LoadFromJSON(const std::string& filepath) {
    nlohmann::json data = ReadJsonFromFile(filepath);

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_group_count_x);
    std::cout << "GL_MAX_COMPUTE_WORK_GROUP_COUNT[0]: " << max_group_count_x << std::endl;

    int max_texture_size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    std::cout << "GL_MAX_TEXTURE_SIZE: " << max_texture_size << std::endl;

    GLint64  msize;
    glGetInteger64i_v(GL_SHADER_STORAGE_BUFFER_SIZE, 1, &msize);
    glGetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &msize);
    std::cout << "GL_SHADER_STORAGE_BUFFER_SIZE: " << msize << std::endl;

    if (!data.contains("shader_programs")) {
        std::cerr << "Error: No shader programs defined in JSON." << std::endl;
        return;
    }

    for (const auto& [name, shaderData] : data["shader_programs"].items()) {
        /*if (!shaderData.contains("vertex") || !shaderData.contains("fragment")) {
            std::cerr << "Skipping shader program '" << name << "' (missing vertex or fragment shader)." << std::endl;
            continue;
        }*/

        std::string vertexPath = shaderData.contains("vertex") ? "shaders/" + shaderData["vertex"].get<std::string>() : "";
        std::string fragmentPath = shaderData.contains("fragment") ? "shaders/" + shaderData["fragment"].get<std::string>() : "";
        std::string geometryPath = shaderData.contains("geometry") ? "shaders/" + shaderData["geometry"].get<std::string>() : "";
        std::string tessControlPath = shaderData.contains("tess_control") ? "shaders/" + shaderData["tess_control"].get<std::string>() : "";
        std::string tessEvalPath = shaderData.contains("tess_eval") ? "shaders/" + shaderData["tess_eval"].get<std::string>() : "";
        std::string computePath = shaderData.contains("compute") ? "shaders/" + shaderData["compute"].get<std::string>() : "";


        GLuint program = CompileShaderProgram(vertexPath, fragmentPath, geometryPath, tessControlPath, tessEvalPath, computePath);
        shader_programs[name] = program;

        CacheUniforms(name);

        std::cout << "Compiled shader program: " << name << std::endl;
    }
    std::string default_shader = Fetch(data, "active_shader", "");
    UseShader(default_shader);
    
    SetUniform("ambient_intensity", static_cast<float>(Fetch(data["uniforms"], "ambient_intensity", 0.02f)));
    SetUniform("debug_mode", 0);

}

void ShaderManager::CacheUniforms(const std::string& shaderName) {
    if (shader_programs.find(shaderName) == shader_programs.end()) return;
    GLuint program = shader_programs[shaderName];
    shader_uniforms[program] = Uniforms(program);
    std::cout << "uniforms cached" << std::endl;
}

void ShaderManager::SetUniform(const std::string& shaderName, const std::string& uniformName, DataType value) {
    if (shader_programs.find(shaderName) == shader_programs.end()) {
        std::cout << "Shader not found: " << shaderName << std::endl;
        return;
    }
    GLuint program = shader_programs[shaderName];
    shader_uniforms[program].FindAndUpdate(uniformName, value);
}

void ShaderManager::SetUniform(const std::string& uniformName, DataType value) {
    if (active_shader == 0) {
        std::cerr << "No active shader program set!" << std::endl;
        return;
    }
    shader_uniforms[active_shader].FindAndUpdate(uniformName, value);
}

void ShaderManager::UseShader(GLuint program) {
    if (program == 0) {
        std::cerr << "Invalid shader program ID: 0" << std::endl;
        return;
    }

    if (active_shader != program) {
        glUseProgram(program);
        active_shader = program;
    }
}


void ShaderManager::UseShader(const std::string& name) {
    if (shader_programs.find(name) == shader_programs.end()) {
        std::cerr << "Shader program not found: " << name << std::endl;
        return;
    }

    GLuint program = shader_programs[name];
    if (active_shader != program) {
        glUseProgram(program);
        active_shader = program;
    }
    //std::cout << "using shader program \"" << name << "\"" << std::endl;
}

GLuint ShaderManager::CompileShaderProgram(const std::string& vertexPath, const std::string& fragmentPath, 
                                           const std::string& geometryPath, 
                                           const std::string& tessControlPath, 
                                           const std::string& tessEvalPath, 
                                           const std::string& computePath) {
    GLuint program = glCreateProgram();
    std::vector<GLuint> shaders;

    auto CompileShader = [](const std::string& path, GLenum type) -> GLuint {
        if (path.empty()) return 0;
        std::ifstream file(path);
        if (!file) {
            std::cerr << "Failed to load shader: " << path << std::endl;
            return 0;
        }
        std::stringstream stream;
        stream << file.rdbuf();
        std::string code = stream.str();
        const char* src = code.c_str();

        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "Shader compilation failed: " << path << "\n" << infoLog << std::endl;
            glDeleteShader(shader);
            return 0;
        }
        std::cout << "returning shader " << path << std::endl;
        return shader;
    };

    // Compile and attach shaders
    shaders.push_back(CompileShader(vertexPath, GL_VERTEX_SHADER));
    shaders.push_back(CompileShader(fragmentPath, GL_FRAGMENT_SHADER));
    shaders.push_back(CompileShader(geometryPath, GL_GEOMETRY_SHADER));
    shaders.push_back(CompileShader(tessControlPath, GL_TESS_CONTROL_SHADER));
    shaders.push_back(CompileShader(tessEvalPath, GL_TESS_EVALUATION_SHADER));
    shaders.push_back(CompileShader(computePath, GL_COMPUTE_SHADER));

    for (GLuint shader : shaders) {
        if (shader != 0) glAttachShader(program, shader);
    }

    glLinkProgram(program);
    
    // Cleanup
    for (GLuint shader : shaders) {
        if (shader != 0) glDeleteShader(shader);
    }

    return program;
}



GLuint ShaderManager::GetShaderID(const std::string& name) {
    return shader_programs.count(name) ? shader_programs[name] : 0;
}

GLuint ShaderManager::GetActiveShader() { return active_shader; }
