#pragma once
#include <chrono>

enum TIMER_STATE { // 0xABC, A = active, B = paused, C = ringing
	TICKING = 0b100,
	PAUSED	= 0b110,
	RINGING = 0b001,
	IDLE	= 0b010,
	SCRAP	= 0b000
};

struct Timer {
	using Chrono = std::chrono::steady_clock;
	using Timestamp = std::chrono::time_point<Chrono>;
	using Duration = std::chrono::duration<float>;

	Timestamp start_time;
	Timestamp end_time;
	Duration length;
	Duration initial_length;
	Timestamp initial_start;

	bool auto_dispose;
	bool auto_dismiss;
	
	TIMER_STATE state;

	Timer(float length_seconds, Timestamp current_time, bool auto_dispose, bool auto_dismiss);

	TIMER_STATE Pause(Timestamp current_time); //bool -> successfully paused? if not consider deletion
	TIMER_STATE Resume(Timestamp current_time);
	TIMER_STATE Evaluate(Timestamp current_time); //true if timer has expired

	Timestamp Expiration();
	Duration Remaining(Timestamp current_time);
	Duration Elapsed(Timestamp current_time);

	TIMER_STATE Reset(Timestamp current_time); //maintains pause state
	TIMER_STATE Restart(Timestamp current_time); //resets and starts

	TIMER_STATE Dismiss();
	TIMER_STATE Dispose();

	void EnableAutoDisposal();
	void DisableAutoDisposal();
	void EnableAutoDismissal();
	void DisableAutoDismissal();
};
