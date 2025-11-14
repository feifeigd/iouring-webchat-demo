#include "ThreadPool.h"
#include <cassert>
#include <iostream>
using namespace std;

ThreadPool::ThreadPool(uint32_t numThreads)  {
    for (uint32_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

bool ThreadPool::empty()const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return tasks.empty();
}

void ThreadPool::stop() {
    if(stop_) {
        return;
    }

    stop_ = true;
    condition.notify_all();
}

void ThreadPool::workerThread() {
    while (true) {
        TaskPtr task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] -> bool { return stop_ || !tasks.empty(); });
            if (stop_ && tasks.empty()) {
                cout << "Worker thread is exiting." << endl;
                return;
            }
            // task = tasks.front();
            task = std::move(tasks.front());
            // assert(task);
            tasks.pop();
            cout << "Worker thread woke up. tasks size=" << tasks.size() << ", threadid=" << this_thread::get_id() << endl;
        }

        // if(task)
        {
            (*task)();
        }
    }
}
