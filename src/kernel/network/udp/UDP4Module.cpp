//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/UDP4SendEvent.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/event/network/UDP4ReceiveEvent.h>
#include "UDP4Module.h"

namespace Kernel {
    UDP4Module::UDP4Module(Kernel::NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
        sockets = new Util::HashMap<uint16_t, UDP4SocketController *>();
        socketAccessLock = new Spinlock();
    }

    uint8_t UDP4Module::registerControllerFor(uint16_t destinationPort, UDP4SocketController *controller) {
        if (destinationPort == 0) {
            log.error("Given port was zero, not registering controller");
            return 1;
        }
        if (controller == nullptr) {
            log.error("Given controller was null, not registering");
            return 1;
        }
        if (sockets == nullptr || socketAccessLock == nullptr) {
            log.error("Socket map or socket access lock not initialized, not registering controller");
            return 1;
        }
        socketAccessLock->acquire();
        if (sockets->containsKey(destinationPort)) {
            socketAccessLock->release();
            //Already registered here
            return 1;
        }
        sockets->put(destinationPort, controller);
        socketAccessLock->release();
        return 0;
    }

    uint8_t UDP4Module::unregisterControllerFor(uint16_t destinationPort) {
        if (destinationPort == 0) {
            log.error("Given port was zero, not unregistering controller");
            return 1;
        }
        if (sockets == nullptr || socketAccessLock == nullptr) {
            log.error("Socket map or socket access lock not initialized, not registering controller");
            return 1;
        }
        socketAccessLock->acquire();
        if (sockets->containsKey(destinationPort)) {
            sockets->remove(destinationPort);
        }
        socketAccessLock->release();
        return 0;
    }

    void UDP4Module::onEvent(const Kernel::Event &event) {
        if (event.getType() == UDP4SendEvent::TYPE) {
            auto *destinationAddress = ((UDP4SendEvent &) event).getDestinationAddress();
            auto *udp4Datagram = ((UDP4SendEvent &) event).getDatagram();
            //TODO: Add null check for internal data structure
            if (udp4Datagram == nullptr) {
                log.error("Outgoing UDP4 datagram was null, ignoring");
                return;
            }
            if (destinationAddress == nullptr) {
                log.error("Destination address was null, discarding message");
                delete udp4Datagram;
                return;
            }
            if (udp4Datagram->getLengthInBytes() == 0) {
                log.error("Outgoing datagram was empty, discarding it");
                delete udp4Datagram;
                return;
            }
            eventBus->publish(
                    new IP4SendEvent(
                            new IP4Datagram(destinationAddress, udp4Datagram)
                    )
            );
            return;
        }
        if (event.getType() == UDP4ReceiveEvent::TYPE) {
            IP4Header *ip4Header = ((UDP4ReceiveEvent &) event).getIP4Header();
            auto *udp4Header = ((UDP4ReceiveEvent &) event).getUDP4Header();
            auto *input = ((UDP4ReceiveEvent &) event).getInput();

            if (
                    udp4Header == nullptr ||
                    ip4Header == nullptr ||
                    input == nullptr ||
                    sockets == nullptr ||
                    socketAccessLock == nullptr
                    ) {
                log.error("One of incoming objects,socket map or socket access lock was null, discarding input");
                delete ip4Header;
                delete input;
                return;
            }

            auto destinationPort = udp4Header->getDestinationPort();
            socketAccessLock->acquire();
            if (!sockets->containsKey(destinationPort)) {
                log.error("No socket registered for datagram's destination port, discarding");
                delete udp4Header;
                delete ip4Header;
                delete input;
                return;
            }

            if (sockets->get(destinationPort)->notifySocket(ip4Header, udp4Header, input)) {
                log.error("Could not deliver input to destination socket");
                delete udp4Header;
                delete ip4Header;
                delete input;
                return;
            }
            socketAccessLock->release();
            return;
        }
    }

}