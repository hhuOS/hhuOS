//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/UDP4SendEvent.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/event/network/UDP4ReceiveEvent.h>
#include "UDP4Module.h"

namespace Kernel {
    //Private method!
    uint8_t UDP4Module::notifyDestinationSocket(UDP4Header *udp4Header, IP4Header *ip4Header, NetworkByteBlock *input) {
        if(sockets== nullptr || accessLock == nullptr){
            log.error("Internal socket list or accessLock not initialized, discarding UDP4 datagram");
            return 1;
        }
        auto destinationPort = udp4Header->getDestinationPort();
        accessLock->acquire();
        if (!sockets->containsKey(destinationPort)) {
            log.error("No socket registered for datagram's destination port, discarding");
            accessLock->release();
            return 1;
        }

        if (sockets->get(destinationPort)->notify(ip4Header, udp4Header, input)) {
            log.error("Could not deliver input to destination socket");
            accessLock->release();
            return 1;
        }
        accessLock->release();
        return 0;
    }

    UDP4Module::UDP4Module(Kernel::NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
        sockets = new Util::HashMap<uint16_t, UDP4SocketController *>();
        accessLock = new Spinlock();
    }

    UDP4Module::~UDP4Module() {
        delete accessLock;
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
        if (sockets == nullptr || accessLock == nullptr) {
            log.error("Socket map or socket access lock not initialized, not registering controller");
            return 1;
        }
        accessLock->acquire();
        if (sockets->containsKey(destinationPort)) {
            accessLock->release();
            //Already registered here
            return 1;
        }
        sockets->put(destinationPort, controller);
        accessLock->release();
        return 0;
    }

    uint8_t UDP4Module::unregisterControllerFor(uint16_t destinationPort) {
        if (destinationPort == 0) {
            log.error("Given port was zero, not unregistering controller");
            return 1;
        }
        if (sockets == nullptr || accessLock == nullptr) {
            log.error("Socket map or socket access lock not initialized, not registering controller");
            return 1;
        }
        accessLock->acquire();
        if (sockets->containsKey(destinationPort)) {
            sockets->remove(destinationPort);
        }
        accessLock->release();
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
            auto *ip4Header = ((UDP4ReceiveEvent &) event).getIP4Header();
            auto *input = ((UDP4ReceiveEvent &) event).getInput();

            if (ip4Header == nullptr || input == nullptr){
                log.error("Incoming IP4Header or input was null, discarding data");
                delete ip4Header;
                delete input;
                return;
            }
            auto *udp4Header = new UDP4Header();
            if (udp4Header->parse(input)) {
                log.error("Parsing UDP4Header failed, discarding");
                delete udp4Header;
                delete ip4Header;
                delete input;
                return;
            }
            if(notifyDestinationSocket(udp4Header, ip4Header, input)){
                log.error("Could not notify destination socket, see syslog for more details");
            }
            //Processing finally done, cleanup
            delete udp4Header;
            delete ip4Header;
            delete input;
            return;
        }
    }

}