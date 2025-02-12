#include "user.h"
#include "json.h"
#include <iostream>
User::User(const std::string& filepath, Program* program) {

	this->program = program;

	nlohmann::json data = ReadJsonFromFile(filepath);

	name = data["name"];

	input = new Input();

}

