//
// Created by hannes on 15.05.21.
//

#include "EthernetSendEvent.h"

Kernel::EthernetSendEvent::EthernetSendEvent(NetworkDevice *outInterface, EthernetFrame *ethernetFrame) : outInterface(
        outInterface), ethernetFrame(ethernetFrame) {}

String Kernel::EthernetSendEvent::getType() const {
    return TYPE;
}
