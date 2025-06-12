#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <filesystem>
#include <vector>
#include <algorithm>
#include "vertex.h"
#include "mesh.h"
#include "window.h"
#include "scene.h"
#include "cursor.h"
#include "input_manager.h"
#include "json.h"
#include "clock.h"
#include "program.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include "callbacks.h"


int main() {
	
	//----------		LOAD CALL			----------//

	std::string program_filepath = "/data/jsons/program.json";

	Program program{program_filepath};
	
	nlohmann::json program_json = ReadJsonFromFile(program_filepath);

	std::string scene_filepath = "/data/jsons/scenes/" + std::string(program_json["scene"]);

	program.scene = new Scene(scene_filepath, program);

	std::cout << "finished loading scene" << std::endl;

	//----------		RENDER CALL			----------//

	program.Run();

	std::cout << "finished rendering scene" << std::endl;
	
	//----------		CLEANUP CALL		----------//
	
	program.Cleanup();

	std::cout << "finished cleanup" << std::endl;

	exit(EXIT_SUCCESS);
}
