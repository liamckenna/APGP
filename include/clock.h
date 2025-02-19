#pragma once
#include <chrono>
#include <vector>
#include "timer.h"

using Timestamp = std::chrono::steady_clock::time_point;

struct Clock {
    
    Timestamp current_time;
    Timestamp start_time;
    Timestamp last_time;
    Timestamp last_fps_time;
    int frame_count;
    float delta_time;
    std::vector<Timer> timers;

    Clock();
    void Tick();
    void UpdateTime();
    void CalculateFrameRate();

    void AddTimer(float length_seconds, bool auto_dispose = true, bool auto_dismiss = true);
    void PollTimers();
    
    float GetElapsedTime() const;
    float GetDeltaTime() const;
};