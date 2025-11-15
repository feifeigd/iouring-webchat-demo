#pragma once

#include <bits/range_access.h> // size
#include <cstdint>

#include <string_view>

class Stream {
    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;
    int fd_{-1};
    char read_buf_[4096];
public:

    Stream(int fd);
    Stream(Stream&& other);
    ~Stream();
    int fd() const { return fd_; }

    char* getReadBuff(){
        return read_buf_;
    }
    
    size_t getReadBuffSize(){
        return std::size(read_buf_);
    }
};
