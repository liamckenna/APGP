#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
#include "transform.h"
#include "vertex.h"
#include "mesh.h"
#include "window.h"
#include "camera.h"
#include "scene.h"
#include "cursor.h"
#include "light.h"
#include "input.h"
#include "user.h"
#include "shaders.h"
#include "json.h"
#include "clock.h"
#include "program.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include "callbacks.h"

//forward declarations of functions
//User* UserGeneration(std::string file); //DEPRECIATED AND DELETED
//Program* ProgramGeneration(std::string program_filepath); //DEPRECIATED AND DELETED
//bool ShaderInitialization(Scene*& scene, nlohmann::json data); //DEPRECIATED AND DELETED
//void DefaultMaterialGeneration(Scene*& scene, nlohmann::json data); //DEPRECIATED AND DELETED
//std::string LoadShader(const char* filepath); //DEPRECIATED AND DELETED
//GLuint createShader(GLenum type, const char* shaderSource); //DEPRECIATED AND DELETED
//bool ProcessShader(Scene*& scene, std::string file, SHADER_TYPE TYPE, bool composite); //DEPRECIATED AND DELETED
//void PollTimers(User*& user, Scene*& scene); //DEPRECIATED AND DELETED
//Scene* SceneGeneration(std::string file); //DEPRECIATED AND DELETED
//void RenderScene(User*& user, Scene*& scene); //DEPRECIATED AND DELETED
//void CameraGeneration(Scene*& scene, nlohmann::json data); //DEPRECIATED AND DELETED
//void ObjectGeneration(Scene*& scene, nlohmann::json data); //DEPRECIATED AND DELETED
//void RenderFullScreenQuad(Scene*& scene); //DEPRECIATED AND DELETED
//void RenderDirectlyToScreen(Scene*& scene); //DEPRECIATED AND DELETED
//void FramebufferInitialization(User*& user, Scene*& scene); //DEPRECIATED AND DELETED
//void BufferArrayInitialization(Scene*& scene); //DEPRECIATED AND DELETED
//void UpdateTextureUniforms(Scene*& scene); //DEPRECIATED AND DELETED
//void Cleanup(User*& user, Scene*& scene); //DEPRECIATED AND DELETED
//void ProcessInput(User*& user, Scene*& scene); //COMMENTED OUT





int main() {
	std::cout << __cplusplus << std::endl;
	//main should create our program object, then do our program loop i think, then cleanup
	std::string program_filepath = "/data/jsons/program.json";

	

	Program* program = new Program(program_filepath);
	nlohmann::json program_json = ReadJsonFromFile(program_filepath);

	std::string scene_filepath = "/data/jsons/scenes/" + std::string(program_json["scene"]);
	program->scene = new Scene(scene_filepath, program);
	std::cout << "initialized scene" << std::endl;

	//----------		RENDER CALL			----------//

	program->Run();

	std::cout << "finished rendering scene" << std::endl;
	
	//----------		CLEANUP CALL		----------//
	
	program->Cleanup();

	std::cout << "finished cleanup" << std::endl;

	exit(EXIT_SUCCESS);
}
/*
void ProcessInput(User*& user, Scene*& scene) {
	Program* program = user->program;
	Clock* clock = program->clock;
	float delta_time = clock->GetDeltaTime();
	if (scene->held_object != nullptr) {
		if (glfwGetKey(user->window->window, GLFW_KEY_W) == GLFW_PRESS) { //w, translate forward
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 0.5f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 2.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity, delta_time);
			}
		}

		
		if (glfwGetKey(user->window->window, GLFW_KEY_S) == GLFW_PRESS) { //s, translate backward
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 0.5f * -1.f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * 2.f * -1.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateForward(scene->main_camera->velocity * -1.f, delta_time);
			}
		}

		if (glfwGetKey(user->window->window, GLFW_KEY_A) == GLFW_PRESS) { //a, translate left
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 0.5f * -1.f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 2.f * -1.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * -1.f, delta_time);
			}
		}

		if (glfwGetKey(user->window->window, GLFW_KEY_D) == GLFW_PRESS) { //d, translate right
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 0.5f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity * 2.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateRight(scene->main_camera->velocity, delta_time);
			}
		}

		if (glfwGetKey(user->window->window, GLFW_KEY_SPACE) == GLFW_PRESS) { //space, translate up
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 0.5f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 2.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity, delta_time);
			}
		}

		if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { //left ctrl, translate down
			if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 0.5f * -1.f, delta_time);
			} else if (glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * 2.f * -1.f, delta_time);
			} else {
				scene->GetObjectByName("camera shell")->t->local.TranslateUp(scene->main_camera->velocity * -1.f, delta_time);
			}
		}
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_LEFT) == GLFW_PRESS) { //left arrow
		if (scene->held_object == scene->GetObjectByName("base")) { //J0
			scene->held_object->t->local.TranslateRight(-5.0f, delta_time);
		} else if (scene->held_object == scene->GetObjectByName("top")) { //J1
			scene->held_object->t->local.RotateYaw(-45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("arm1")) { //J2
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("arm2")) { //J3
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("pen") && glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) { //J4
			scene->held_object->parent->parent->t->local.RotateYaw(-45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("pen")) { //J6
			scene->held_object->t->local.RotateRoll(-45.f * delta_time);
		}
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_RIGHT) == GLFW_PRESS) { //right arrow
		if (scene->held_object == scene->GetObjectByName("base")) { //J0
			scene->held_object->t->local.TranslateRight(5.0f, delta_time);
		} else if (scene->held_object == scene->GetObjectByName("top")) { //J1
			scene->held_object->t->local.RotateYaw(45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("arm1")) { //J2
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("arm2")) { //J3
			//no action requried
		} else if (scene->held_object == scene->GetObjectByName("pen") && glfwGetKey(user->window->window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) { //J4
			scene->held_object->parent->parent->t->local.RotateYaw(45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("pen")) { //J6
			scene->held_object->t->local.RotateRoll(45.f * delta_time);
		}
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_DOWN) == GLFW_PRESS) { //down arrow
		if (scene->held_object == scene->GetObjectByName("base")) { //J0
			scene->held_object->t->local.TranslateForward(-5.0f, delta_time);
		} else if (scene->held_object == scene->GetObjectByName("top")) { //J1
			//no action required
		} else if (scene->held_object == scene->GetObjectByName("arm1")) { //J2
			scene->held_object->t->local.RotatePitch(-45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("arm2")) { //J3
			scene->held_object->parent->t->local.RotatePitch(-45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("pen")) { //J5
			scene->held_object->parent->t->local.RotatePitch(-45.f * delta_time);
		}
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_UP) == GLFW_PRESS) { //up arrow
		if (scene->held_object == scene->GetObjectByName("base")) { //J0
			scene->held_object->t->local.TranslateForward(5.0f, delta_time);
		} else if (scene->held_object == scene->GetObjectByName("top")) { //J1
			//no action required
		} else if (scene->held_object == scene->GetObjectByName("arm1")) { //J2
			scene->held_object->t->local.RotatePitch(45.f * delta_time);
		} else if (scene->held_object == scene->GetObjectByName("arm2")) { //J3
			scene->held_object->parent->t->local.RotatePitch(45.f * delta_time);
		}  else if (scene->held_object == scene->GetObjectByName("pen")) { //J5
			scene->held_object->parent->t->local.RotatePitch(45.f * delta_time);
		}
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_COMMA) == GLFW_PRESS) { //comma ","
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_PERIOD) == GLFW_PRESS) { //period "."
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { //escape "esc"
		glfwSetWindowShouldClose(user->window->window, GL_TRUE);
	}
	//p2
	if (glfwGetKey(user->window->window, GLFW_KEY_B) == GLFW_PRESS) { //b
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_T) == GLFW_PRESS) { //t
		if (user->timers.size() == 0) {
			std::cout << "adding timer" << std::endl;
			user->timers.push_back(new Timer(10.f));
		}
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_Y) == GLFW_PRESS) { //y
		user->timers.push_back(new Timer(5.f, false));
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_U) == GLFW_PRESS) { //u
		std::cout << "timers in vector: " << user->timers.size() << std::endl;
		for (auto& timer : user->timers) {
			if (timer->ringing)
				if (timer->Silence()) std::cout << "silenced timer" << std::endl;
		}
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_I) == GLFW_PRESS) { //i
		float current = glfwGetTime();
		for (auto& timer : user->timers) {
			std::cout << timer->Remaining(current) << std::endl;
		}
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_1) == GLFW_PRESS) { //one "1"
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_2) == GLFW_PRESS) { //two "2"
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_3) == GLFW_PRESS) { //three "3"
		scene->shading_mode = 1;
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_4) == GLFW_PRESS) { //four "4"
		scene->shading_mode = 2;
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_5) == GLFW_PRESS) { //five "5"
		scene->shading_mode = 3;
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_6) == GLFW_PRESS) { //six "6"
		scene->shading_mode = 4;
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_7) == GLFW_PRESS) { //six "6"
		scene->shading_mode = 5;
	}

	if (glfwGetMouseButton(user->window->window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS) { //mouse button 5 (far thumb button)
		scene->GetObjectByName("quadrant lights")->SetActiveLocalTree(true); //turn on quadrant lights
	}
	if (glfwGetMouseButton(user->window->window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS) { //mouse button 4 (close thumb button)
		scene->GetObjectByName("quadrant lights")->SetActiveLocalTree(false); //turn off quadrant lights
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS) { //caps lock
		scene->PrintObjectTrees();
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_F1) == GLFW_PRESS) { //function 1 "f1"
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_F) == GLFW_PRESS) { //f
		dynamic_cast<Light*>(scene->GetObjectByName("camera shell")->GetChildByNameTree("camera")->GetChildByNameTree("flashlight"))->SetActiveLocalTree(true); //turn on flashlight
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_G) == GLFW_PRESS) { //g
		dynamic_cast<Light*>(scene->GetObjectByName("camera shell")->GetChildByNameTree("camera")->GetChildByNameTree("flashlight"))->SetActiveLocalTree(false); //turn off flashlight
	}
	
	if (glfwGetKey(user->window->window, GLFW_KEY_L) == GLFW_PRESS) { //l
		if (scene->GetMeshByName("face")->draw_mode != GL_TRIANGLES) {
			scene->GetMeshByName("face")->SetDrawMode("GL_TRIANGLES");
			UpdateMeshBuffer(scene, scene->GetMeshByName("face"));
		}
		
	}
	if (glfwGetKey(user->window->window, GLFW_KEY_SEMICOLON) == GLFW_PRESS) { //;
		if (scene->GetMeshByName("face")->draw_mode != GL_LINES) {
			scene->GetMeshByName("face")->SetDrawMode("GL_LINES");
			UpdateMeshBuffer(scene, scene->GetMeshByName("face"));
		}
	}

}
*/