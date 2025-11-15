#pragma once

#include "IoUringProcessor.h"

#include <functional>
#include <thread>
#include <vector>

class Application;
class IoUringLoop {
    IoUringLoop(const IoUringLoop&) = delete;
    IoUringLoop& operator=(const IoUringLoop&) = delete;

    uint32_t numThreads_;
    uint32_t index_mask_;
    std::vector<IoUringProcessor> processors_;
    std::vector<std::jthread> threads_;
    static int nextHandle_;

    int generateHandle();
    IoUringProcessor& getProcessor(int handle);
    Application& app_;
public:

    IoUringLoop(Application& app, uint32_t numThreads = std::thread::hardware_concurrency());
    ~IoUringLoop();

    bool start();
    void stop();

    int listen(uint16_t port, Listener::OnNewClient onNewClient);
    void createStream(int fd, Listener::OnNewClient onNewClient);
    void close(int handle);

private:
    void createListener(int handle, int fd, uint16_t port, Listener::OnNewClient onNewClient);
};
