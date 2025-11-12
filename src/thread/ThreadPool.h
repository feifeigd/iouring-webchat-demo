#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <condition_variable>
#include <atomic>

class ThreadPool {
    using Task = std::function<void()>;
public:

    ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();

    void enqueue(Task task);
private:
    void workerThread();
    std::vector<std::jthread> workers;
    std::queue<Task> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};
