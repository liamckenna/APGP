#include "util.h"
#include <iostream>
#include <filesystem>

std::string GetExecutableDirectory() {
	return std::filesystem::current_path().string();
}