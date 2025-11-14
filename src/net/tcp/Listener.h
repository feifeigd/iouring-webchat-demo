#pragma once

#include <netinet/in.h> // sockaddr_in

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
    // 这两个参数由 accept 回写，要保证生命周期，如果同时投递多个accept，则需要多组参数
    struct sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
public:


    Listener(int fd, uint16_t port, OnNewClient onNewClient);
    Listener(Listener&& other);
    ~Listener();
    int fd() const { return fd_; }
};

