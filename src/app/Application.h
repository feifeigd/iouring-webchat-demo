#pragma once

#include "thread/ThreadPool.h"
#include <chrono>

class Application{
    ThreadPool threadPool_;
    bool running_{true};
public:
    Application();
    ~Application() = default;

    void run();
    virtual void shutdown();

private:
    virtual void initialize();
};
