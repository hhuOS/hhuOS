//
// Created by hannes on 15.05.21.
//

#include "EthernetSendEvent.h"

namespace Kernel {

    EthernetSendEvent::EthernetSendEvent(NetworkDevice *outInterface, EthernetFrame *ethernetFrame)
            : outInterface(
            outInterface), ethernetFrame(ethernetFrame) {}

    String EthernetSendEvent::getType() const {
        return TYPE;
    }

    NetworkDevice *EthernetSendEvent::getOutInterface() const {
        return outInterface;
    }

    EthernetFrame *EthernetSendEvent::getEthernetFrame() const {
        return ethernetFrame;
    }

}