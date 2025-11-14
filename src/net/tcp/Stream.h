#pragma once

class Stream {
    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;
    int fd_{-1};
public:

    Stream(int fd);
    ~Stream();
    int fd() const { return fd_; }
};
