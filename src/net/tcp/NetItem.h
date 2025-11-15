#pragma once

class NetItem{
    NetItem(const NetItem&) = delete;
    NetItem& operator=(const NetItem&) = delete;
    int fd_{-1};
public:
    NetItem(int fd): fd_{fd}{

    }
    NetItem(NetItem&& other): fd_{other.fd_}{
        other.fd_ = -1;
    }

    virtual ~NetItem(){
        close();
    }

    int fd()const{
        return fd_;
    }
    void close();
};
