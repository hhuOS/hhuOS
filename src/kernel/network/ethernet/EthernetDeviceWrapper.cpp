//
// Created by hannes on 25.05.21.
//

#include "EthernetDeviceWrapper.h"

EthernetDeviceWrapper::EthernetDeviceWrapper(NetworkDevice *networkDevice) {
    uint8_t macAddress[6]{0,0,0,0,0,0};
    networkDevice->getMacAddress(macAddress);
    this->ethernetAddress=new EthernetAddress(
            macAddress[0],
            macAddress[1],
            macAddress[2],
            macAddress[3],
            macAddress[4],
            macAddress[4]
            );
}

EthernetAddress *EthernetDeviceWrapper::getEthernetAddress() {
    return ethernetAddress;
}

void EthernetDeviceWrapper::sendEthernetFrame(EthernetFrame *ethernetFrame) {
    networkDevice->sendPacket(ethernetFrame->getDataAsByteBlock(),ethernetFrame->getLength());
}
