//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/UDP4ReceiveEvent.h>
#include <kernel/event/network/UDP4SendEvent.h>
#include <kernel/event/network/IP4SendEvent.h>
#include "UDP4Module.h"

namespace Kernel {

    UDP4Module::UDP4Module(Kernel::NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
        socketManager = new UDP4SocketManager();
    }

    uint8_t UDP4Module::registerController(UDP4SocketController *controller) {
        if(controller== nullptr){
            log.error("Given controller was null, not registering");
            return 1;
        }
        if(socketManager == nullptr){
            log.error("Socket manager not initialized, not registering controller");
            return 1;
        }
        return socketManager->addController(controller);
    }

    uint8_t UDP4Module::unregisterController(UDP4Port *port) {
        if(port== nullptr){
            log.error("Given port was null, not unregistering controller");
            return 1;
        }
        if(socketManager == nullptr){
            log.error("Socket manager not initialized, not unregistering controller");
            return 1;
        }
        return socketManager->removeController(port);
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
            auto *udp4Datagram = ((UDP4ReceiveEvent &) event).getUDP4Datagram();
            auto *ip4Datagram = ((UDP4ReceiveEvent &) event).getIP4Datagram();
            auto *input = ((UDP4ReceiveEvent &) event).getInput();

            if (udp4Datagram == nullptr) {
                log.error("Incoming UDP4Datagram was null, discarding input");
                delete ip4Datagram;
                delete input;
                return;
            }
            if (ip4Datagram == nullptr) {
                log.error("Incoming IP4Datagram was null, discarding input");
                delete udp4Datagram;
                delete input;
                return;
            }
            if (input == nullptr) {
                log.error("Incoming input was null, discarding datagrams");
                delete udp4Datagram;
                delete ip4Datagram;
                return;
            }

            if(socketManager== nullptr){
                log.error("Internal socket manager was null, discarding incoming data");
                delete udp4Datagram;
                delete ip4Datagram;
                delete input;
                return;
            }

            if(socketManager->notifyDestinationSocket(ip4Datagram, udp4Datagram, input)){
                log.error("Could not deliver input to destination socket");
            }

            //udp4Datagram is not part of ip4Datagram here
            //-> we need to delete it separately!
            delete ip4Datagram;
            delete udp4Datagram;

            //Input processing done, cleanup
            delete input;
            return;
        }
    }

}