#pragma once

#include "net/tcp/Listener.h"
#include "net/tcp/Stream.h"

#include <liburing.h>
#include <unordered_map>

class IoUringLoop;
class IoUringProcessor {
    IoUringProcessor(const IoUringProcessor&) = delete;
    IoUringProcessor& operator=(const IoUringProcessor&) = delete;
    
    friend class IoUringLoop;
    IoUringLoop& loop_;
    
    uint32_t id_{};
    io_uring ring_{}; // 要求内核 Linux 5.1+
    int entries_{1024}; // [1, 4096] 而且必须是2的N次方
    std::unordered_map<int, Listener> listeners_;
    std::unordered_map<int, Stream> streams_;
public:
    IoUringProcessor(IoUringLoop& loop, uint32_t id);
    ~IoUringProcessor();

    IoUringProcessor(IoUringProcessor&& other);

    bool init();
    void runLoop();
    int submit_accept(Listener& listener);
    int submit_read(Stream& stream);
    int submit_write(int handle, Stream& stream);
private:
    void addListener(int handle, Listener&& listener);
    void removeListener(int handle);

    void addStream(int handle, Stream&& stream);
    void removeStream(int handle);
};
