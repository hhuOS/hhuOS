//
// Created by hannes on 14.06.21.
//

#ifndef HHUOS_UDP4SOCKETMANAGER_H
#define HHUOS_UDP4SOCKETMANAGER_H


#include <kernel/network/internet/IP4Datagram.h>
#include "UDP4SocketController.h"
#include <lib/util/HashMap.h>
#include "UDP4Port.h"
#include "UDP4Socket.h"

namespace Kernel {
    class UDP4SocketManager {
    private:
        Util::HashMap<UDP4Port *, UDP4SocketController *> *sockets;

    public:
        UDP4SocketManager();

        uint8_t notifyDestinationSocket(IP4Datagram *ip4Datagram, UDP4Datagram *udp4Datagram, NetworkByteBlock *input);

        uint8_t addController(UDP4SocketController *attributes);

        uint8_t removeController(UDP4Port *port);
    };
}

#endif //HHUOS_UDP4SOCKETMANAGER_H
