#include "timer.h"

Timer::Timer() {
	start = initial_start = glfwGetTime();
	length = initial_length = 0.f;
	one_time_use = true;
	this->active = true;
	this->ringing = false;
	this->paused = false;
}

Timer::Timer(float length, float start, bool one_time_use) {
	this->start = this->initial_start = start;
	this->length = this->initial_length = length;
	this->one_time_use = one_time_use;
	this->active = true;
	this->ringing = false;
	this->paused = false;
}

bool Timer::Pause(float current) {
	if (Evaluate(current)) return false;
	paused = true;
	length += start - current;
	return true;
}
bool Timer::Resume(float current) {
	if (!paused) return false;
	paused = false;
	start = current;
	return true;
}

bool Timer::Evaluate(float current) {
	if (paused || !active) return false;
	if ((start + length) - current > 0) return false;
	else {
		active = false;
		ringing = true;
		return true;
	}
}

float Timer::Expiration() {
	if (paused) return -1.f; //unknown while paused
	else return start + length;
}

float Timer::Remaining(float current, bool despite_pause) {
	if (paused) {
		if (despite_pause) return length;	//user wants the remaining time with the understanding the timer is paused
		else return -1.f;					//and therefore is not representative of when the timer is due to go off.. 
	} else {								//if they do not explicitly accept this risk, we disallow the retrieval.
		return start + length - current;
	}
}

float Timer::Elapsed(float current) {
	return initial_length - Remaining(current, true);
}

bool Timer::Reset(float current) {
	if (!active) active = true;
	start = current;
	length = initial_length;
	Silence();
	return true;
}

bool Timer::Silence() {
	if (ringing) {
		ringing = false;
		return true;
	}
	else {
		return false;
	}
}