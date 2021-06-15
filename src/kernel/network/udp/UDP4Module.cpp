//
// Created by hannes on 17.05.21.
//

#include "UDP4Module.h"

namespace Kernel {
    UDP4Module::UDP4Module(Kernel::NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
        sockets = new Util::HashMap<UDP4Port *, UDP4SocketController *>();
    }

    uint8_t UDP4Module::registerControllerFor(UDP4Port *destinationPort, UDP4SocketController *controller) {
        if(controller== nullptr){
            log.error("Given controller was null, not registering");
            return 1;
        }
        if(sockets == nullptr){
            log.error("Socket map not initialized, not registering controller");
            return 1;
        }
        sockets->put(destinationPort,controller);
        return 0;
    }

    uint8_t UDP4Module::unregisterControllerFor(UDP4Port *destinationPort) {
        if(destinationPort == nullptr){
            log.error("Given port was null, not unregistering controller");
            return 1;
        }
        if(sockets == nullptr){
            log.error("Socket map not initialized, not unregistering controller");
            return 1;
        }
        if(sockets->containsKey(destinationPort)) {
            sockets->remove(destinationPort);
        }
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
            //Address has been copied inside constructor, can be deleted now
            delete destinationAddress;
            return;
        }
        if (event.getType() == UDP4ReceiveEvent::TYPE) {
            auto *udp4Header = ((UDP4ReceiveEvent &) event).getUDP4Datagram();
            auto *ip4Header = ((UDP4ReceiveEvent &) event).getIP4Datagram();
            auto *input = ((UDP4ReceiveEvent &) event).getInput();

            if (udp4Header == nullptr) {
                log.error("Incoming UDP4Datagram was null, discarding input");
                delete ip4Header;
                delete input;
                return;
            }
            if (ip4Header == nullptr) {
                log.error("Incoming IP4Datagram was null, discarding input");
                delete udp4Header;
                delete input;
                return;
            }
            if (input == nullptr) {
                log.error("Incoming input was null, discarding datagrams");
                delete udp4Header;
                delete ip4Header;
                return;
            }

            if(sockets == nullptr){
                log.error("Internal socket map was null, discarding incoming data");
                delete udp4Header;
                delete ip4Header;
                delete input;
                return;
            }

            auto *destinationPort = udp4Header->getDestinationPort();
            if(!sockets->containsKey(destinationPort)){
                log.error("No socket registered for datagram's destination port, discarding");
                delete udp4Header;
                delete ip4Header;
                delete input;
                return;
            }

            if(sockets->get(destinationPort)->notifySocket(input)){
                log.error("Could not deliver input to destination socket");
            }

            //udp4Datagram is not part of ip4Datagram here
            //-> we need to delete it separately!
            delete ip4Header;
            delete udp4Header;

            //Input processing done, cleanup
            delete input;
            return;
        }
    }

}