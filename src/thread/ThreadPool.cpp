#include "ThreadPool.h"
#include <cassert>

ThreadPool::ThreadPool(size_t numThreads)  {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    stop.store(true);
    condition.notify_all();
}


void ThreadPool::workerThread() {
    while (true) {
        TaskPtr task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] -> bool { return stop.load() || !tasks.empty(); });
            if (stop.load() && tasks.empty()) {
                return;
            }
            // task = tasks.front();
            task = std::move(tasks.front());
            // assert(task);
            tasks.pop();
        }

        // if(task)
        {
            (*task)();
        }
    }
}
