#pragma once

#include "net/Buffer.h"
#include <memory>

class Protocol{
protected:
    Buffer buff_;
public:
    int append(char const* data, size_t len);
    char const* data()const{
        return buff_.dataBegin();
    }
    size_t dataLen()const{
        return buff_.dataSize();
    }

    Buffer& getBuffer(){
        return buff_;
    }
    virtual int parseData() = 0;
    virtual std::shared_ptr<Buffer> unpack() = 0;
    virtual Buffer pack(char const* data, size_t len) = 0;
    virtual std::shared_ptr<Protocol> clone() = 0;
};
