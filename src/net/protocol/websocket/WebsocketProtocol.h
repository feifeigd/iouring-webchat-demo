#pragma once

#include "../Protocol.h"

class WebsocketProtocol : public Protocol{
public:
    int parseData()override;
    std::shared_ptr<Buffer> unpack()override;
    Buffer pack(char const* data, size_t len)override;
    std::shared_ptr<Protocol> clone()override;
};
