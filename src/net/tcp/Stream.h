#pragma once

#include "CommitData.h"
#include "NetItem.h"

#include <bits/range_access.h> // size
#include <cstdint>


class Stream : public NetItem{
    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;
    int handle_{-1};
    
    CommitData commitDataRead_{CommitType::READ, this, {}};
    CommitData commitDataWrite_{CommitType::WRITE, this, {}};
    char read_buf_[4096];
    char write_buf_[4096];
    int write_size_{};
public:

    Stream(int handle, int fd);
    Stream(Stream&& other);
    ~Stream();

    int handle()const{
        return handle_;
    }

    char* getReadBuff(){
        return read_buf_;
    }

    char* getWriteBuff(){
        return read_buf_;
    }
    
    std::size_t getReadBuffSize(){
        return std::size(read_buf_);
    }

    std::size_t getWriteBuffSize(){
        return std::size(write_buf_);
    }

    std::size_t getWriteSize()const{
        return write_size_;
    }

    void write(void* data, int size);

    CommitData& getCommitDataRead(){
        return commitDataRead_;
    }
    CommitData& getCommitDataWrite(){
        return commitDataWrite_;
    }

    void resetWriteBuff(){
        write_size_ = 0;
    }
};
