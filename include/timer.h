#pragma once
#include <GLFW/glfw3.h>


struct Timer {
	float start;
	float length;
	float initial_length;
	float initial_start;
	bool one_time_use;
	bool paused;
	bool active; //here, active means potentially running, paused or not
	bool ringing;
	Timer();
	Timer(float length, float start = glfwGetTime(), bool one_time_use = true);
	bool Pause(float current = glfwGetTime()); //bool -> successfully paused? if not consider deletion
	bool Resume(float current = glfwGetTime());
	bool Evaluate(float current = glfwGetTime()); //bool refers to whether or not the timer is finished
	//we want minimize the number of gettime calls by sharing a current time between timers, ie passing in a preretrieved time

	float Expiration();
	float Remaining(float current = glfwGetTime(), bool despite_pause = false);
	float Elapsed(float current = glfwGetTime());
	//dynamically calculate these when requested rather than storing and constantly updating them

	bool Reset(float current = glfwGetTime());
	bool Silence();
};
