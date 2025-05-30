#pragma once
#include "cursor.h"
#include "byte.h"
#include "timer.h"
#include <unordered_set>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

enum KEY_STATE {
	UP			= 0b00, //inactive now, unchanged
	PRESSED		= 0b11, //active now,	changed
	DOWN		= 0b10, //active now,	unchanged
	RELEASED	= 0b01	//inactive now,	changed
};
struct Program;
struct InputManager {

	Program& program;

	byte key_states[GLFW_KEY_LAST / 4];

	std::unordered_set<int> active_key_stack; //keys with recent activity that need to transition states

	Cursor cursor;

	struct Wheel {
		float dx = 0;
		float dy = 0;
		bool active = false;
	} wheel;

	Timer idle_mouse;

	InputManager(Program& program);

	void UpdateKeyStack();

	bool UpdateKeyState(int key, bool pressed);

	void UpdateAllKeyStates(GLFWwindow* window);

	KEY_STATE GetKeyState(int key);
};