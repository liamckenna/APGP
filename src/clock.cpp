#include "clock.h"
#include <iostream>

Clock::Clock() {
    start_time = std::chrono::high_resolution_clock::now();
    last_time = start_time;
    last_fps_time = start_time;
    frame_count = 0;
    delta_time = 0.f;
}

void Clock::UpdateDeltaTime() {
    auto current_time = std::chrono::high_resolution_clock::now();
    delta_time = std::chrono::duration<float>(current_time - last_time).count();
    last_time = current_time;
}

void Clock::CalculateFrameRate() {
    auto current_time = std::chrono::high_resolution_clock::now();
    frame_count++;

    float elapsed = std::chrono::duration<float>(current_time - last_fps_time).count();
    if (elapsed >= 1.0f) {
        double fps = frame_count / elapsed;
        std::cout << "FPS: " << fps << std::endl;
        last_fps_time = current_time;
        frame_count = 0;
    }
}

float Clock::GetElapsedTime() const {
    return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start_time).count();
}

float Clock::GetDeltaTime() const {
    return delta_time;
}