#include "json.h"
#include <fstream>
#include <iostream>
std::unique_ptr<nlohmann::json> ReadJsonPointerFromFile(const std::string& filename) {
	std::string absolute_filepath = std::filesystem::current_path().string() + filename;
	std::ifstream file(absolute_filepath, std::ifstream::binary);
	auto root = std::make_unique<nlohmann::json>();
	if (file.is_open()) {
		file >> *root;
		file.close();
	}
	return root;
}

nlohmann::json ReadJsonFromFile(const std::string& filename) {

	std::string absolute_filepath = std::filesystem::current_path().string() + filename;
	std::ifstream file(absolute_filepath, std::ifstream::binary);
	if (!file.is_open()) {

		return nlohmann::json(); 
	}


	nlohmann::json root;


	try {
		file >> root;
	} catch (const std::exception& e) {

		std::cout << "Error reading JSON data: " << e.what() << std::endl;
		file.close();
		return nlohmann::json(); 
	}


	file.close();



	return root;
}
