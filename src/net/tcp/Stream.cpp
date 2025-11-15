#include "Stream.h"

#include <unistd.h>

Stream::Stream(int fd): fd_{fd}{

}

Stream::Stream(Stream&& other): fd_{other.fd_}
{
    other.fd_ = -1;
}

Stream::~Stream(){
    if(fd_ != -1){
        close(fd_);
    }
}
