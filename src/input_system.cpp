#include "input_system.h"
#include <iostream>
#include <algorithm>
#include "program.h"
#include "clock.h"
#include "byte.h"
#include "window.h"
InputSystem::InputSystem(InputManager& im) : input_manager(im) {};

void InputSystem::Update(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
	
	Entity camera = component_manager.GetEntitiesWithComponent<PrimaryCameraComponent>()[0];
	TransformComponent& transform = component_manager.GetComponent<TransformComponent>(camera);

	byte space = input_manager.GetKeyState(GLFW_KEY_SPACE);
	byte w = input_manager.GetKeyState(GLFW_KEY_W);
	byte a = input_manager.GetKeyState(GLFW_KEY_A);
	byte s = input_manager.GetKeyState(GLFW_KEY_S);
	byte d = input_manager.GetKeyState(GLFW_KEY_D);
	byte one = input_manager.GetKeyState(GLFW_KEY_1);
	byte two = input_manager.GetKeyState(GLFW_KEY_2);
	byte lctrl = input_manager.GetKeyState(GLFW_KEY_LEFT_CONTROL);
	byte lmb = input_manager.GetKeyState(GLFW_MOUSE_BUTTON_LEFT);
	byte rmb = input_manager.GetKeyState(GLFW_MOUSE_BUTTON_RIGHT);
	byte mmb = input_manager.GetKeyState(GLFW_MOUSE_BUTTON_MIDDLE);


	switch (space) {
	case PRESSED:
	case DOWN:
		transform.TranslateUp(5, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (lctrl) {
	case PRESSED:
	case DOWN:
		transform.TranslateDown(5, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (w) {
	case PRESSED:
	case DOWN:
		transform.TranslateForward(5, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (a) {
	case PRESSED:
	case DOWN:
		transform.TranslateLeft(5, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (s) {
	case PRESSED:
	case DOWN:
		transform.TranslateBackward(5, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (d) {
	case PRESSED:
	case DOWN:
		transform.TranslateRight(3, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (lmb) {
	case PRESSED:
		double xpos, ypos;
		glfwGetCursorPos(input_manager.cursor.current_window->glfw_window, &xpos, &ypos);
		input_manager.cursor.Update(xpos, ypos);
		glfwSetInputMode(input_manager.cursor.current_window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	case DOWN:
		if (input_manager.cursor.dx != 0 || input_manager.cursor.dy != 0) {
			transform.RotateYaw(-input_manager.cursor.dx, 1);
			transform.RotatePitch(input_manager.cursor.dy, 1);
			input_manager.cursor.dx = 0;
			input_manager.cursor.dy = 0;
		}
		break;
	case RELEASED:
		glfwSetInputMode(input_manager.cursor.current_window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case UP:
		break;
	}
	switch (rmb) {
	case PRESSED:
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (one) {
	case PRESSED:
		input_manager.program.shader_manager.SetUniform("debug_mode", 0);
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (two) {
	case PRESSED:
		input_manager.program.shader_manager.SetUniform("debug_mode", 1);
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	
	


	if (input_manager.wheel.active) {
		if (input_manager.wheel.dy != 0) {
			for (auto entity : component_manager.GetEntitiesWithComponent<LightComponent>()) {
				LightComponent& light = component_manager.GetComponent<LightComponent>(entity);
				light.intensity += (input_manager.wheel.dy * delta_time * 1000.f);
				light.intensity = std::max(light.intensity, 0.f);
				light.stale = true;
			}
		}
	}

	input_manager.UpdateKeyStack(); //HAS TO BE AT THE END
}
/*
void ProcessInput(User*& user, Scene*& scene) {
	Program& program = user->program;
	Clock* clock = program.clock;
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