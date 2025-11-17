#pragma once

#include "net/protocol/Protocol.h"

class NetItem{
    NetItem(const NetItem&) = delete;
    NetItem& operator=(const NetItem&) = delete;

    int fd_{-1};
    int handle_{-1};
    bool closing_{};
    int pending_count_{};
protected:
    std::shared_ptr<Protocol> protocol_;
public:
    NetItem(int fd, int handle, std::shared_ptr<Protocol> protocol)
        : fd_{fd}, handle_{handle}
        , protocol_{protocol}
    {

    }

    NetItem(NetItem&& other)
        : fd_{other.fd_}, handle_{other.handle_}
        , closing_{other.closing_}, pending_count_{other.pending_count_}
        , protocol_{other.protocol_}
    {
        other.fd_ = -1;
        other.handle_ = -1;
        other.closing_ = false;
        other.pending_count_ = 0;
    }

    virtual ~NetItem(){
        close();
        handle_ = -1;
    }

    int fd()const{
        return fd_;
    }

    int handle()const{
        return handle_;
    }

    bool closing()const{
        return closing_;
    }
    int pending_count()const{
        return pending_count_;
    }
    int add_pending_count(int count = 1){
        return pending_count_ += count;
    }
    int sub_pending_count(){
        return --pending_count_;
    }
    virtual void close();

    void setClosing(bool value);
};
