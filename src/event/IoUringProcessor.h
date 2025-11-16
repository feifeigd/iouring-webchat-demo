#pragma once

#include "net/tcp/Listener.h"
#include "net/tcp/Stream.h"

#include <liburing.h>
#include <sys/eventfd.h>
#include <unordered_map>

class IoUringLoop;
class IoUringProcessor {
    enum WakeupType{
        NEW_LISTENER,
    };
    IoUringProcessor(const IoUringProcessor&) = delete;
    IoUringProcessor& operator=(const IoUringProcessor&) = delete;
    
    friend class IoUringLoop;
    IoUringLoop& loop_;
    
    uint32_t id_{};
    io_uring ring_{}; // 要求内核 Linux 5.1+
    int entries_{1024}; // [1, 4096] 而且必须是2的N次方
    std::unordered_map<int, Listener> listeners_;
    std::unordered_map<int, Stream> streams_;

    union{
        int pipe_fds_[2]; // 用于唤醒线程
        struct{
            int pipe_read_;
            int pipe_write_;
        };
    };

    struct WakeupData{
        uint64_t type{};
        uint64_t param{};
    }wakeupData_;

    std::mutex mutex_;
    std::unordered_map<int, Listener> pending_listeners_;
public:
    IoUringProcessor(IoUringLoop& loop, uint32_t id);
    ~IoUringProcessor();

    IoUringProcessor(IoUringProcessor&& other);

    bool init();
    void runLoop();
    int submit_accept(Listener& listener);
    int submit_read(Stream& stream);
    int submit_write(int handle, Stream& stream);
    void wakeup_io_uring(WakeupType, uint64_t param = 0);
private:
    void addListener(int handle, Listener&& listener);
    void removeListener(int handle);

    void addStream(int handle, Stream&& stream);
    void removeStream(int handle);

    void submit_wakeup_read();
    void handle_wakeup_event();

    void handle_io_completion(struct io_uring_cqe* cqe);
};
