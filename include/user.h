#pragma once

#include "input.h"
#include <GLFW/glfw3.h>

struct Program;
struct User { //local user variables all packaged in one convenient place
	
	Program* program;
	std::string name;
	Input* input;

	User(const std::string& filepath, Program* program = nullptr);
};