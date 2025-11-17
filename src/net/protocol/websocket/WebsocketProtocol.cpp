#include "WebsocketProtocol.h"

int WebsocketProtocol::parseData(){
    return 0;
}

std::shared_ptr<Buffer> WebsocketProtocol::unpack(){
    return {};
}

Buffer WebsocketProtocol::pack(char const* data, size_t len){
    return {};
}

std::shared_ptr<Protocol> WebsocketProtocol::clone() {
    return {};
}
