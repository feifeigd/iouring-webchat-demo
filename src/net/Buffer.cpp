#include "Buffer.h"

#include <algorithm>
#include <cstring> // memmove

constexpr size_t MIN_BUFF_SIZE = 32;

size_t nextPowerOf2(size_t n){
    if(MIN_BUFF_SIZE >= n){
        return MIN_BUFF_SIZE;
    }
    if(!(n & (n - 1))){
        return n;
    }

    auto size = MIN_BUFF_SIZE;
    while(size < n){
        size <<= 1;
    }
    return size;
}

Buffer::Buffer(char const* data, size_t len){
    capacity_ = nextPowerOf2(len + 1);
    buff_ = new char[capacity_];
    std::copy(data, data + len, buff_);
    buff_[len] = 0;
    data_size_ = len;
}

Buffer::~Buffer(){
    if(buff_){
        delete[] buff_;
        buff_ = nullptr;
    }
    capacity_ = 0;
    data_size_ = 0;
    read_pos_ = 0;
}

void Buffer::forward(size_t size){
    if(size > data_size_){
        size = data_size_;
    }

    read_pos_ += size;
    data_size_ -= size;
}


int Buffer::read(void* to_buff, int len){
    if(data_size_ < len){
        return -1;
    }
    auto data_begin = dataBegin();
    std::copy(data_begin, data_begin + len, to_buff);
    forward(len);
    return len;
}

size_t Buffer::write(char const* data, size_t len){
    auto end_space = capacity_ - (data_size_ + read_pos_); // 尾部未写入的空间
    if(end_space <= len){
        enlarge(len);
    }
    std::copy(data, data + len, buff_ + read_pos_ + data_size_);
    data_size_ += len;
    buff_[read_pos_ + data_size_] = 0;
    return len;
}

void Buffer::enlarge(size_t len){
    auto const free_size = capacity_ - data_size_;
    if(free_size < len){
        auto next_size = nextPowerOf2(data_size_ + len + 1);
        auto next_buf = new char[next_size];
        auto data_begin = dataBegin();
        std::copy(data_begin, data_begin + data_size_, next_buf);
        next_buf[data_size_] = 0;

        delete[] buff_;
        buff_ = next_buf;

        read_pos_ = 0;
        capacity_ = next_size;
    }else{
        // 移到开头
        memmove(buff_, buff_ + read_pos_, data_size_);
        read_pos_ = 0;
    }
}
