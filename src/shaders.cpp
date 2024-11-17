#include "shaders.h"

void Shaders::CleanupShaders() {
	glDeleteProgram(shader_program);
}