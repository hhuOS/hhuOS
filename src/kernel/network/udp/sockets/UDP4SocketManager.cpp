//
// Created by hannes on 14.06.21.
//

#include "UDP4SocketManager.h"

namespace Kernel {
    UDP4SocketManager::UDP4SocketManager() {
        sockets = new Util::HashMap<UDP4Port *, UDP4SocketController *>();
    }

    uint8_t UDP4SocketManager::addController(UDP4SocketController *controller) {
        if (controller == nullptr) {
            return 1;
        }
        sockets->put(controller->getListeningPort(), controller);
        return 0;
    }

    uint8_t UDP4SocketManager::removeController(UDP4Port *port) {
        if (port == nullptr) {
            return 1;
        }
        if (sockets->containsKey(port)) {
            sockets->remove(port);
        }
        return 0;
    }

    uint8_t UDP4SocketManager::notifyDestinationSocket(IP4Datagram *ip4Datagram, UDP4Datagram *udp4Datagram,
                                                       NetworkByteBlock *input) {
        if (
                sockets == nullptr ||
                ip4Datagram == nullptr ||
                udp4Datagram == nullptr ||
                input == nullptr ||
                !sockets->containsKey(udp4Datagram->getDestinationPort())
                ) {
            return 1;
        }

        //TODO: Check contents with checksums!
        return sockets->get(udp4Datagram->getDestinationPort())->process(input);
    }
}