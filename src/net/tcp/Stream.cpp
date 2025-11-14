#include "Stream.h"

#include <unistd.h>

Stream::Stream(int fd): fd_{fd}{

}
Stream::~Stream(){
    if(fd_ != -1){
        close(fd_);
    }
}
