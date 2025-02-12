#include "timer.h"
using Chrono = std::chrono::steady_clock;
using Timestamp = std::chrono::time_point<Chrono>;
using Duration = std::chrono::duration<float>;

inline Timestamp::duration DurationCast(Duration duration) {
	return std::chrono::duration_cast<Timestamp::duration>(duration);
}


Timer::Timer(float length_seconds, Timestamp current_time, bool auto_dispose, bool auto_dismiss) {
	start_time = current_time;
	initial_start = start_time;
	length = Duration(length_seconds);
	initial_length = length;
	this->auto_dispose = auto_dispose;
	this->auto_dismiss = auto_dismiss;
	end_time = Timestamp();
	state = TICKING;
}

TIMER_STATE Timer::Pause(Timestamp current_time) {
	switch (state) {
	case TICKING:
		length -= current_time - start_time;
		state = PAUSED;
		break;
	case RINGING: //functioning as a dismiss
		if (auto_dispose) state = SCRAP;
		else state = IDLE;
		break;
	case PAUSED:
	case IDLE:
	case SCRAP:
		break;
	}
	return state;
}
TIMER_STATE Timer::Resume(Timestamp current_time) {
	switch (state) {
	case PAUSED:
		start_time = current_time;
		state = TICKING;
		break;
	case TICKING:
	case RINGING:
	case IDLE:
	case SCRAP:
		break;
	}
	return state;
}

TIMER_STATE Timer::Evaluate(Timestamp current_time) {
	switch (state) {
	case TICKING:
		if (current_time >= start_time + DurationCast(length)) {
			end_time = current_time;
			if (auto_dismiss) {
				if (auto_dispose) state = SCRAP;
				else state = IDLE;
			}
			else state = RINGING;
		}
		break;
	case PAUSED:
	case RINGING:
	case IDLE:
	case SCRAP:
		break;
	}
	return state;
}

Timestamp Timer::Expiration() {
	switch (state) {
	case TICKING:
		return start_time + DurationCast(length);
	case PAUSED:
		return Timestamp();
	case RINGING:
	case IDLE:
	case SCRAP:
		return end_time;
	}
}

Duration Timer::Remaining(Timestamp current_time) {
	switch (state) {
	case TICKING:
		return start_time + DurationCast(length) - current_time;
	case PAUSED:
		return length;
	case RINGING:
	case IDLE:
	case SCRAP:
		return Duration::zero();
	}

}

Duration Timer::Elapsed(Timestamp current_time) {
	return initial_length - Remaining(current_time);
}

TIMER_STATE Timer::Reset(Timestamp current_time) {
	switch (state) {
	case TICKING:
		start_time = current_time;
		length = initial_length;
		break;
	case PAUSED:
		length = initial_length;
		break;
	case RINGING:
	case IDLE:
	case SCRAP:
		length = initial_length;
		state = PAUSED;
		break;
	}
	return state;
}

TIMER_STATE Timer::Restart(Timestamp current_time) {
	length = initial_length;
	start_time = current_time;
	state = TICKING;
	return state;
}

TIMER_STATE Timer::Dismiss() {
	switch (state) {
	case TICKING:
	case PAUSED:
	case RINGING:
		if (auto_dispose) state = SCRAP;
		else state = IDLE;
		break;
	case IDLE:
	case SCRAP:
		break;
	}
	return state;
}

TIMER_STATE Timer::Dispose() {
	switch (state) {
	case TICKING:
	case PAUSED:
	case RINGING:
	case IDLE:
	case SCRAP:
		state = SCRAP;
		break;
	}
	return state;
}

void Timer::EnableAutoDisposal() {
	auto_dispose = true;
	if (state == IDLE) state = SCRAP;
}

void Timer::DisableAutoDisposal() {
	auto_dispose = false;
	if (state == SCRAP) state = IDLE;
}

void Timer::EnableAutoDismissal() {
	auto_dismiss = true;
	if (state == RINGING) {
		if (auto_dispose) state = SCRAP;
		else state = IDLE;
	}
}

void Timer::DisableAutoDismissal() {
	auto_dismiss = false;
}