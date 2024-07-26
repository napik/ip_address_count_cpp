#pragma once

#include <chrono>
#include <string>

class Timer {
public:
    Timer() noexcept;

    void start() noexcept;

    void elapsed() const noexcept;

private:
    std::chrono::high_resolution_clock::time_point start_time;

    static void printStartTime() noexcept;

    static std::string formatTimePoint(const std::chrono::system_clock::time_point &tp) noexcept;
};
