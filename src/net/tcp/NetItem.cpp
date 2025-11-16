#include "NetItem.h"

#include <unistd.h>
#include <cerrno>

void NetItem::close(){
    int a = 0;
    if(fd_ >= 0){
        fd_ = -1;
    }
}

void NetItem::setClosing(bool value){
    closing_ = value;
}
