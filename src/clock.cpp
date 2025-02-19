#include "clock.h"
#include <iostream>

Clock::Clock() {
    current_time = std::chrono::steady_clock::now();
    start_time = current_time;
    last_time = current_time;
    last_fps_time = current_time;
    frame_count = 0;
    delta_time = 0.f;
}

void Clock::Tick() {
    UpdateTime();
    CalculateFrameRate();
    PollTimers();
}

void Clock::UpdateTime() {
    last_time = current_time;
    current_time = std::chrono::steady_clock::now();
    delta_time = std::chrono::duration<float>(current_time - last_time).count();
}

void Clock::CalculateFrameRate() {
    frame_count++;
    float elapsed = std::chrono::duration<float>(current_time - last_fps_time).count();
    if (elapsed >= 1.0f) {
        double fps = frame_count / elapsed;
        std::cout << "FPS: " << fps << std::endl;
        last_fps_time = current_time;
        frame_count = 0;
    }
}

void Clock::AddTimer(float length_seconds, bool auto_dispose, bool auto_dismiss) {
    timers.emplace_back(length_seconds, current_time, auto_dispose, auto_dismiss);
}



void Clock::PollTimers() {

    for (auto it = timers.begin(); it != timers.end();) {
        TIMER_STATE state = it->Evaluate(current_time);
        switch (state) {
        case RINGING:
            std::cout << "ring!" << std::endl;
            ++it;
            break;
        case SCRAP:
            it = timers.erase(it);
            break;
        case TICKING:
        case PAUSED:
        case IDLE:
            ++it;
            break;

        }
    }
}

float Clock::GetElapsedTime() const {
    return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start_time).count();
}

float Clock::GetDeltaTime() const {
    return delta_time;
}