#pragma once

#include "event/IoUringLoop.h"
#include "thread/Queue.hpp"
#include "thread/ThreadPool.h"
#include <chrono>

class Application{
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;
    
    ThreadPool threadPool_;
    bool running_{true};
    IoUringLoop ioUringLoop_;
    using Task = std::function<void()>;
    Queue<Task> mainTasks_;
public:
    Application();
    ~Application() = default;

    void push(Task&& task){
        mainTasks_.enqueue(std::move(task));
    }
    
    int run();
    virtual void shutdown();
private:
    virtual void initialize();
};
