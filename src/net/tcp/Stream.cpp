#include "Stream.h"

#include <algorithm>
#include <unistd.h>

Stream::Stream(int handle, int fd, std::shared_ptr<Protocol> protocol)
    : NetItem{fd, handle, protocol}
{

}

Stream::Stream(Stream&& other)
    : NetItem{std::move(other)}
{
}

Stream::~Stream(){
}

void Stream::write(void* data, int size){
    auto write_buff = getWriteBuff();
    size = std::min(size, (int)getWriteBuffSize());
    std::copy((char*)data, (char*)data + size, write_buff);
    write_size_ += size;
}
