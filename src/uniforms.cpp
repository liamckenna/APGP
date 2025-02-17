#include "uniforms.h"
#include "shaders.h"
#include <stdexcept>
using DataType = std::variant<int, unsigned int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;

Uniforms::Uniforms() {
    shaders = nullptr;
}

Uniforms::Uniforms(Shaders* shaders) {
	this->shaders = shaders;
}

Uniform& Uniforms::GetByName(std::string name) {
	auto it = uniforms.find(name);
	if (it == uniforms.end()) {
		throw std::runtime_error("Uniform not found: " + name);
	}
	return it->second;
}

void Uniforms::FindAndUpdate(std::string name, DataType value) {
	try {
		Uniform& uniform = GetByName(name);
		uniform.Update(value);
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
	}

}
void Uniforms::CacheUniforms() {
    GLint uniformCount;
    glGetProgramiv(shaders->gl_program, GL_ACTIVE_UNIFORMS, &uniformCount);

    std::cout << "Total active uniforms: " << uniformCount << std::endl;

    // **Cache Regular Uniforms (NOT UBOs)**
    for (GLint i = 0; i < uniformCount; i++) {
        char name[256];
        GLsizei length;
        GLint size;
        GLenum type;
        glGetActiveUniform(shaders->gl_program, i, sizeof(name), &length, &size, &type, name);

        std::string name_str = name;

        GLint location = glGetUniformLocation(shaders->gl_program, name);
        if (location == -1) {
            // Skip uniforms inside UBOs
            continue;
        }

        size_t bracket_pos = name_str.find("[0]");
        if (bracket_pos != std::string::npos) {
            name_str = name_str.substr(0, bracket_pos);
        }

        uniforms[name_str] = Uniform(name_str, location, type);
        std::cout << "Cached uniform: " << name_str << " at location " << location << std::endl;
    }

    // **Cache Uniform Buffer Objects (UBOs)**
    GLint blockCount;
    glGetProgramiv(shaders->gl_program, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);

    std::cout << "Total active UBOs: " << blockCount << std::endl;

    for (GLint i = 0; i < blockCount; i++) {
        char blockName[256];
        GLsizei length;
        glGetActiveUniformBlockName(shaders->gl_program, i, sizeof(blockName), &length, blockName);

        GLuint blockIndex = glGetUniformBlockIndex(shaders->gl_program, blockName);
        if (blockIndex == GL_INVALID_INDEX) {
            std::cerr << "Failed to get UBO index for " << blockName << std::endl;
            continue;
        }

        // Bind UBO to binding slot i
        glUniformBlockBinding(shaders->gl_program, blockIndex, i);
        std::cout << "Cached UBO: " << blockName << " at block index " << blockIndex << std::endl;
    }
}

