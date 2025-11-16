#pragma once

#include <cstddef>

class Buffer{
    char* buff_{};
    size_t capacity_{};
    size_t data_size_{}; // 未读取的数量
    size_t read_pos_{}; // 下次读取位置
public:
    Buffer() = default;
    // 复制一块数据
    Buffer(char const* data, size_t len);
    ~Buffer();

    // 返回 -1 表示 to_buff 空间不足
    int read(void* to_buff, int len);
    size_t write(char const* data, size_t len);

    size_t dataSize()const{
        return data_size_;
    }

    // read_pos_ 跳过一段数据，
    void forward(size_t size);
    
    void enlarge(size_t len);
    char const* dataBegin()const{
        return buff_ + read_pos_;
    }
};
