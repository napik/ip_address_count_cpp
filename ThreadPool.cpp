#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

ThreadPool::ThreadPool(unsigned int numThreads) {
    workers.reserve(numThreads);
    for (unsigned int i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::scoped_lock lock{queueMutex};
        shutdown = true;
    }
    condition.notify_all();
    for (auto &worker: workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::enqueue(std::function<void()> func) noexcept {
    {
        std::scoped_lock lock{queueMutex};
        if (shutdown) {
            std::cerr << "enqueue on stopped ThreadPool" << std::endl;
            return;
        }
        tasks.emplace(std::move(func));
    }
    condition.notify_one();
}

void ThreadPool::waitUntilAllTasksDone() noexcept {
    std::unique_lock lock{allTasksDoneMutex};
    condition.wait(lock, [this] { return tasks.empty(); });
}

void ThreadPool::workerThread() noexcept {
    while (true) {
        std::packaged_task<void()> task;
        {
            std::unique_lock lock{queueMutex};
            condition.wait(lock, [this] { return shutdown || !tasks.empty(); });
            if (shutdown && tasks.empty()) return;
            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}
