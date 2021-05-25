//
// Created by hannes on 15.05.21.
//

#include "EthernetSendEvent.h"

namespace Kernel {

    EthernetSendEvent::EthernetSendEvent(EthernetDevice *outDevice, EthernetFrame *ethernetFrame)
            : outDevice(outDevice), ethernetFrame(ethernetFrame) {}

    String EthernetSendEvent::getType() const {
        return TYPE;
    }

    EthernetDevice *EthernetSendEvent::getOutDevice() const {
        return outDevice;
    }

    EthernetFrame *EthernetSendEvent::getEthernetFrame() const {
        return ethernetFrame;
    }

}