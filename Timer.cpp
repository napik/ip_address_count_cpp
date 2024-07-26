#include "Timer.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <string>

Timer::Timer() noexcept = default;

void Timer::start() noexcept {
    printStartTime();
    start_time = std::chrono::high_resolution_clock::now();
}

void Timer::elapsed() const noexcept {
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    std::cout << "Complete in " << duration.count() << " seconds" << std::endl;
}

void Timer::printStartTime() noexcept {
    auto now = std::chrono::system_clock::now();
    std::cout << "Start time: " << formatTimePoint(now) << std::endl;
}

std::string Timer::formatTimePoint(const std::chrono::system_clock::time_point &tp) noexcept {
    auto time_t_tp = std::chrono::system_clock::to_time_t(tp);
    std::tm tm{};
    gmtime_r(&time_t_tp, &tm);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
