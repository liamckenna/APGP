#include "input_manager.h"
#include "cursor.h"
#include "windows.h"
#include "program.h"
#include <algorithm>
#include <iostream>
InputManager::InputManager(Program& program) : program(program), cursor() {
	std::memset(key_states, 0, sizeof(key_states));
}

bool InputManager::UpdateKeyState(int key, bool pressed) {
	int index = key / 4;
	int offset = (key % 4) * 2;
	byte mask = 0b11 << offset;

	KEY_STATE previous_state = static_cast<KEY_STATE> ((key_states[index] & mask) >> offset);
	KEY_STATE new_state;

	if (pressed) {
		if (previous_state == UP || previous_state == RELEASED) {
			new_state = PRESSED;
			active_key_stack.insert(key);
		}
		else {
			new_state = DOWN;
		}
	}
	else {
		if (previous_state == DOWN || previous_state == PRESSED) {
			new_state = RELEASED;
			active_key_stack.insert(key);
		}
		else {
			new_state = UP;
		}
	}

	key_states[index] = (key_states[index] & ~mask) | (new_state << offset);

	return new_state != previous_state; //to see if state was changed
}

KEY_STATE InputManager::GetKeyState(int key) {
	int index = key / 4;
	int offset = (key % 4) * 2;
	byte mask = 0b11 << offset;

	return static_cast<KEY_STATE> ((key_states[index] & mask) >> offset);
}

void InputManager::UpdateKeyStack() {
	std::unordered_set<int> current_stack = std::move(active_key_stack);

	for (int key : current_stack) {
		UpdateKeyState(key, ((GetKeyState(key) >> 1) & 0b1));
	}
}

void InputManager::UpdateAllKeyStates(GLFWwindow* window) {
	active_key_stack.clear();

	for (int key = 0; key < GLFW_KEY_LAST; key++) {
		KEY_STATE state = GetKeyState(key);
		bool pressed = (glfwGetKey(window, key) == GLFW_PRESS);

		switch (state) {
		case DOWN:
			if (!pressed) UpdateKeyState(key, false);
			break;
		case UP:
			if (pressed) UpdateKeyState(key, true);
			break;
		case PRESSED:
			if (pressed) active_key_stack.insert(key);
			else UpdateKeyState(key, false);
			break;
		case RELEASED:
			if (pressed) UpdateKeyState(key, true);
			else active_key_stack.insert(key);
			break;
		}
	}
		
}