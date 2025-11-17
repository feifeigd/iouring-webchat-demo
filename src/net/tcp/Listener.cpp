#include "Listener.h"


Listener::Listener(int fd, int handle, uint16_t port, std::shared_ptr<Protocol> protocol, OnNewClient onNewClient)
    : NetItem{fd, handle, protocol}
    ,port_{port}
    , onNewClient_{std::move(onNewClient)}
{

}

Listener::Listener(Listener&& other)
    : NetItem{std::move(other)}, port_{other.port_}
    , onNewClient_{std::move(other.onNewClient_)}
{
    other.port_ = 0;
}
