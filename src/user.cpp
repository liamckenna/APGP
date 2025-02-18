#include "user.h"
#include "program.h"
#include "json.h"
#include <iostream>

User::User(const std::string& filepath, Program* program) : program(program), input() {

	nlohmann::json data = ReadJsonFromFile(filepath);
	name = data["name"];

}

Input& User::GetInput() {
	return input;
}
