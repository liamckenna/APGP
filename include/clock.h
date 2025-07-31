#pragma once
#include <chrono>
#include <vector>

using Timestamp = std::chrono::steady_clock::time_point;

struct Clock {
    
    Timestamp current_time;
    Timestamp start_time;
    Timestamp last_time;
    Timestamp last_fps_time;
    int frame_count;
    float delta_time;
    long long frames;
    long long seconds;

    Clock();
    void Tick();
    void UpdateTime();
    void CalculateFrameRate();
    
    float GetElapsedTime() const;
    float GetDeltaTime() const;
    float GetFinalFPS() const;
};