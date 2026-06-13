#include "precise_timer.h"
#include <thread>

// CREDIT: Claude Opus 4.8

extern "C" {
    __declspec(dllimport) void*         __stdcall CreateWaitableTimerExW(void*, const wchar_t*, unsigned long, unsigned long);
    __declspec(dllimport) int           __stdcall SetWaitableTimer(void*, const long long*, long, void*, void*, int);
    __declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void*, unsigned long);
    __declspec(dllimport) int           __stdcall CloseHandle(void*);
}

namespace {
    constexpr unsigned long CREATE_WAITABLE_TIMER_HIGH_RESOLUTION = 0x00000002;
    constexpr unsigned long TIMER_ALL_ACCESS = 0x1F0003;
    constexpr unsigned long WIN_INFINITE = 0xFFFFFFFFul;
}

PreciseTimer::PreciseTimer() {
    handle = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (!handle) handle = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
}

PreciseTimer::~PreciseTimer() {
    if (handle) CloseHandle(handle);
}

void PreciseTimer::WaitUntil(std::chrono::steady_clock::time_point target) {
    using namespace std::chrono;
    auto coarse_target = target - spin_margin;
    auto now = steady_clock::now();

    if (coarse_target > now) {
        if (handle) {
            long long hundred_ns = duration_cast<duration<long long, std::ratio<1, 10'000'000>>>(coarse_target - now).count();
            if (hundred_ns > 0) {
                long long due = -hundred_ns;
                if (SetWaitableTimer(handle, &due, 0, nullptr, nullptr, 0))
                    WaitForSingleObject(handle, WIN_INFINITE);
            }
        }
        else {
            std::this_thread::sleep_until(coarse_target);
        }
    }

    while (steady_clock::now() < target) { }
}
