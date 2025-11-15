#include "Stream.h"

#include <algorithm>
#include <unistd.h>

Stream::Stream(int handle, int fd): NetItem{fd}, handle_{handle}{

}

Stream::Stream(Stream&& other)
    : NetItem{std::move(other)}
    , handle_{other.handle_}
{
    other.handle_ = -1;
}

Stream::~Stream(){
}

void Stream::write(void* data, int size){
    auto write_buff = getWriteBuff();
    size = std::min(size, (int)getWriteBuffSize());
    std::copy((char*)data, (char*)data + size, write_buff);
    write_size_ += size;
}
