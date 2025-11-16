#include "Listener.h"


Listener::Listener(int fd, int handle, uint16_t port, OnNewClient onNewClient)
    : NetItem{fd, handle}
    ,port_{port}
    , onNewClient_{std::move(onNewClient)}
{

}

Listener::Listener(Listener&& other): NetItem{std::move(other)}, port_{other.port_}, onNewClient_{std::move(other.onNewClient_)}{
    other.port_ = 0;
}
