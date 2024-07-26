#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <future>

class ThreadPool {
public:
    ThreadPool();

    explicit ThreadPool(unsigned int numThreads);

    ~ThreadPool();

    ThreadPool(const ThreadPool &) = delete;

    ThreadPool &operator=(const ThreadPool &) = delete;

    void enqueue(std::function<void()> func) noexcept;

    void waitUntilAllTasksDone() noexcept;

private:
    std::vector<std::thread> workers;
    std::queue<std::packaged_task<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> shutdown{false};
    std::condition_variable allTasksDone;
    std::mutex allTasksDoneMutex;

    void workerThread() noexcept;
};
