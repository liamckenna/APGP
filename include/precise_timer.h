#pragma once
#include <chrono>

// CREDIT: Claude Opus 4.8

struct PreciseTimer {
    void* handle = nullptr;
    std::chrono::microseconds spin_margin{ 500 };

    PreciseTimer();
    ~PreciseTimer();
    PreciseTimer(const PreciseTimer&) = delete;
    PreciseTimer& operator=(const PreciseTimer&) = delete;

    void WaitUntil(std::chrono::steady_clock::time_point target);
};
