#pragma once
#include "cursor.h"
#include "byte.h"
#include <unordered_set>

enum KEY_STATE {
	UP			= 0b00, //inactive now, unchanged
	PRESSED		= 0b11, //active now,	changed
	DOWN		= 0b10, //active now,	unchanged
	RELEASED	= 0b01	//inactive now,	changed
};

struct Input {

	Cursor* cursor;

	byte key_states[GLFW_KEY_LAST / 4];

	std::unordered_set<int> active_key_stack; //keys with recent activity that need to transition states

	Input(Window* window, float mouse_sensitivity);

	void UpdateKeyStack();

	bool UpdateKeyState(int key, bool pressed);

	void UpdateAllKeyStates(GLFWwindow* window);

	KEY_STATE GetKeyState(int key);
};