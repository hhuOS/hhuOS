//
// Created by hannes on 15.05.21.
//

#include "EthernetSendEvent.h"

namespace Kernel {
    EthernetSendEvent::EthernetSendEvent(EthernetDevice *outDevice, EthernetAddress *targetHardwareAddress,
                                         EthernetDataPart *dataPart)
            : outDevice(outDevice), targetHardwareAddress(targetHardwareAddress), dataPart(dataPart) {}

    String EthernetSendEvent::getType() const {
        return TYPE;
    }

    EthernetDevice *EthernetSendEvent::getOutDevice() const {
        return outDevice;
    }

    EthernetAddress *EthernetSendEvent::getTargetHardwareAddress() const {
        return targetHardwareAddress;
    }

    EthernetDataPart *EthernetSendEvent::getDataPart() const {
        return dataPart;
    }
}