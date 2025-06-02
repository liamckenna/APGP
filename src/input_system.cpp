#include "input_system.h"
#include <iostream>
#include <algorithm>
#include "program.h"
#include "clock.h"
#include "byte.h"
#include "window.h"
#include "render_system.h"
#define SPEED 3
InputSystem::InputSystem(InputManager& im) : input_manager(im) {};

void InputSystem::Update(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
	
	Entity camera = component_manager.GetEntitiesWithComponent<PrimaryCameraComponent>()[0];
	TransformComponent& transform = component_manager.GetComponent<TransformComponent>(camera);

	byte esc = input_manager.GetKeyState(GLFW_KEY_ESCAPE);
	byte space = input_manager.GetKeyState(GLFW_KEY_SPACE);
	byte tab = input_manager.GetKeyState(GLFW_KEY_TAB);
	byte lshift = input_manager.GetKeyState(GLFW_KEY_LEFT_SHIFT);
	byte lctrl = input_manager.GetKeyState(GLFW_KEY_LEFT_CONTROL);

	byte w = input_manager.GetKeyState(GLFW_KEY_W);
	byte a = input_manager.GetKeyState(GLFW_KEY_A);
	byte s = input_manager.GetKeyState(GLFW_KEY_S);
	byte d = input_manager.GetKeyState(GLFW_KEY_D);

	byte one = input_manager.GetKeyState(GLFW_KEY_1);
	byte two = input_manager.GetKeyState(GLFW_KEY_2);
	byte three = input_manager.GetKeyState(GLFW_KEY_3);
	byte four = input_manager.GetKeyState(GLFW_KEY_4);
	byte five = input_manager.GetKeyState(GLFW_KEY_5);

		
	byte lmb = input_manager.GetKeyState(GLFW_MOUSE_BUTTON_LEFT);
	byte rmb = input_manager.GetKeyState(GLFW_MOUSE_BUTTON_RIGHT);
	byte mmb = input_manager.GetKeyState(GLFW_MOUSE_BUTTON_MIDDLE);

	byte f = input_manager.GetKeyState(GLFW_KEY_F);

	switch (esc) { //escape
	case PRESSED:
		if (glfwGetInputMode(input_manager.cursor.current_window->glfw_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			glfwSetInputMode(input_manager.cursor.current_window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (space) { //space
	case PRESSED:
	case DOWN:
		transform.TranslateUp(SPEED, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (lctrl) { //left control
	case PRESSED:
	case DOWN:
		transform.TranslateDown(SPEED, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (tab) { //left control
	case PRESSED:
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}

	switch (w) { //w
	case PRESSED:
	case DOWN:
		transform.TranslateForward(SPEED, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (a) { //a
	case PRESSED:
	case DOWN:
		transform.TranslateLeft(SPEED, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (s) { //s
	case PRESSED:
	case DOWN:
		transform.TranslateBackward(SPEED, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (d) { //d
	case PRESSED:
	case DOWN:
		transform.TranslateRight(SPEED, delta_time);
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}

	switch (lmb) { //left mouse button/mouse button 1
	case PRESSED:
		if (glfwGetInputMode(input_manager.cursor.current_window->glfw_window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
			double xpos, ypos;
			glfwGetCursorPos(input_manager.cursor.current_window->glfw_window, &xpos, &ypos);
			input_manager.cursor.Update(xpos, ypos);
			glfwSetInputMode(input_manager.cursor.current_window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			break;
		}
		else {

		}
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	if (glfwGetInputMode(input_manager.cursor.current_window->glfw_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
		if (input_manager.cursor.dx != 0 || input_manager.cursor.dy != 0) {
			transform.RotateYaw(-input_manager.cursor.dx, 1);
			transform.RotatePitch(input_manager.cursor.dy, 1);
			input_manager.cursor.dx = 0;
			input_manager.cursor.dy = 0;
		}
	}
	
	switch (rmb) {  //right mouse button/mouse button 3
	case PRESSED:
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}

	switch (one) { //1
	case PRESSED:
		if (lshift == DOWN) input_manager.program.shader_manager.UseShader("debug_quad");
		input_manager.program.shader_manager.SetUniform("debug_mode", 0);
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (two) { //2
	case PRESSED:
		if (lshift == DOWN) input_manager.program.shader_manager.UseShader("debug_quad");
		input_manager.program.shader_manager.SetUniform("debug_mode", 1);
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (three) { //3
	case PRESSED:
		if (lshift == DOWN) input_manager.program.shader_manager.UseShader("debug_quad");
		input_manager.program.shader_manager.SetUniform("debug_mode", 2);
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (four) { //4
	case PRESSED:
		if (lshift == DOWN) input_manager.program.shader_manager.UseShader("debug_quad");
		input_manager.program.shader_manager.SetUniform("debug_mode", 3);
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}
	switch (five) { //5
	case PRESSED:
		if (lshift == DOWN) input_manager.program.shader_manager.UseShader("debug_quad");
		input_manager.program.shader_manager.SetUniform("debug_mode", 4);
		break;
	case DOWN:
		break;
	case RELEASED:
		break;
	case UP:
		break;
	}

	switch (f) { //f
	case PRESSED:
		for (auto entity : component_manager.GetEntitiesWithComponent<LightComponent>()) {
			TransformComponent& light_transform = component_manager.GetComponent<TransformComponent>(entity);
			light_transform.SetPosition(transform.position);
			LightComponent& light = component_manager.GetComponent<LightComponent>(entity);
			light.stale = true;
			ParaboloidPointLightComponent& point_light = component_manager.GetComponent<ParaboloidPointLightComponent>(entity);
			point_light.stale = true;
			break;
		}
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
