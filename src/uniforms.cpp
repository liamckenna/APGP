#include "uniforms.h"
#include <iostream>
#include <stdexcept>
using DataType = std::variant<int, unsigned int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;


Uniforms::Uniforms(GLuint program) {
    this->program = program;

    // --- Regular Uniforms ---
    GLint uniformCount;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);

    std::cout << "Caching uniforms for program " << program << ", found " << uniformCount << " uniforms." << std::endl;

    for (GLint i = 0; i < uniformCount; i++) {
        char name[256];
        GLsizei length;
        GLint size;
        GLenum type;
        glGetActiveUniform(program, i, sizeof(name), &length, &size, &type, name);

        std::string nameStr = name;
        GLint location = glGetUniformLocation(program, name);
        if (name == "model") std::cout << "HIIHHIFHIOAWEFAIFA" << std::endl;
        // **Handle UBO-stored uniforms** (they have location = -1)
        if (location == -1) {
            continue;  // Skip for now, but we may handle them differently later
        }

        // **Fix array uniforms (e.g., "lights[0]" -> "lights")**
        size_t bracket_pos = nameStr.find("[0]");
        if (bracket_pos != std::string::npos) {
            nameStr = nameStr.substr(0, bracket_pos);
        }

        uniforms[nameStr] = Uniform(nameStr, location, type);
        std::cout << "Cached uniform: " << nameStr << " at location " << location << std::endl;
    }

    // --- Uniform Buffer Objects (UBOs) ---
    GLint blockCount;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);

    std::cout << "Total active UBOs: " << blockCount << std::endl;

    for (GLint i = 0; i < blockCount; i++) {
        char blockName[256];
        GLsizei length;
        glGetActiveUniformBlockName(program, i, sizeof(blockName), &length, blockName);

        GLuint blockIndex = glGetUniformBlockIndex(program, blockName);
        if (blockIndex == GL_INVALID_INDEX) {
            std::cerr << "Failed to get UBO index for " << blockName << std::endl;
            continue;
        }

        glUniformBlockBinding(program, blockIndex, i);
        std::cout << "Cached UBO: " << blockName << " at block index " << blockIndex << std::endl;
    }
}



Uniform& Uniforms::GetByName(const std::string& name) {
	auto it = uniforms.find(name);
	if (it == uniforms.end()) {
		throw std::runtime_error("Uniform not found: " + name);
	}
	return it->second;
}

void Uniforms::FindAndUpdate(const std::string& name, DataType value) {
    if (uniforms.find(name) == uniforms.end()) {
        std::cerr << "Uniform not found: " << name << std::endl;
        //std::cerr << "Printing out list of all uniforms" << std::endl;
        for (auto& uniform : uniforms) {
            //std::cout << uniform.second.name << std::endl;
        }
        return;
    }
    uniforms[name].Update(value);
}
