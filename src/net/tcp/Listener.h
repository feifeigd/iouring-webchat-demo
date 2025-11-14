#pragma once

#include <functional>
#include <cstdint>

class Listener {
public:
    using OnNewClient = std::function<void(int handle)>;

private:
    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;

    friend class IoUringProcessor;

    int fd_{-1};
    uint16_t port_{0};
    bool pendingClose_{};
    
    OnNewClient onNewClient_;
public:


    Listener(int fd, uint16_t port, OnNewClient onNewClient);
    Listener(Listener&& other);
    ~Listener();
    int fd() const { return fd_; }
};

