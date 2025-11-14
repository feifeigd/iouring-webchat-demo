#include "Listener.h"

#include <unistd.h>

Listener::Listener(int fd, uint16_t port, OnNewClient onNewClient)
    : fd_{fd}
    , port_{port}
    , onNewClient_{std::move(onNewClient)}
{

}

Listener::Listener(Listener&& other): fd_{other.fd_}, port_{other.port_}, onNewClient_{std::move(other.onNewClient_)}{
    other.fd_ = -1;
}

Listener::~Listener(){
    if(fd_ < 0){
        return;
    }
    close(fd_);
    fd_ = -1;
}
