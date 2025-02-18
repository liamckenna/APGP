#pragma once
#include "input.h"

struct Program;

 //local user variables all packaged in one convenient place

struct User { 
	
	Program* program;
	std::string name;
	
	Input input;

	User(const std::string& filepath, Program* program = nullptr);

	Input& GetInput();
};