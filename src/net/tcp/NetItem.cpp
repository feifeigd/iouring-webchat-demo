#include "NetItem.h"

#include <unistd.h>


void NetItem::close(){
    if(fd_ >= 0){
        ::close(fd_);
        fd_ = -1;
    }
}
