#pragma once

#include "net/tcp/Listener.h"
#include "net/tcp/Stream.h"

#include <liburing.h>
#include <unordered_map>

class IoUringProcessor {
    IoUringProcessor(const IoUringProcessor&) = delete;
    IoUringProcessor& operator=(const IoUringProcessor&) = delete;

    friend class IoUringLoop;
    uint32_t id_{};
    io_uring ring_{}; // 要求内核 Linux 5.1+
    int entries_{1024}; // [1, 4096] 而且必须是2的N次方
    std::unordered_map<int, Listener> listeners_;
    std::unordered_map<int, Stream> streams_;
public:
    IoUringProcessor(uint32_t id);
    ~IoUringProcessor();

    IoUringProcessor(IoUringProcessor&& other);

    bool init();
    void runLoop();
private:
    void createListener(int handle, int fd, uint16_t port, Listener::OnNewClient onNewClient);
    void removeListener(int handle);

    void removeStream(int handle);
};
