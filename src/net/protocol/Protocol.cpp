#include "Protocol.h"

int Protocol::append(char const* data, size_t len){
    buff_.write(data, len);
    return parseData();
}
