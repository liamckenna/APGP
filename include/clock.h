#pragma once

#include <chrono>

struct Clock {
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point last_time;
    std::chrono::high_resolution_clock::time_point last_fps_time;
    int frame_count;
    float delta_time;

    Clock();
    void UpdateDeltaTime();
    void CalculateFrameRate();
    float GetElapsedTime() const;
    float GetDeltaTime() const;
};