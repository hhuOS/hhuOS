//
// Created by hannes on 26.05.21.
//

#include "EthernetDevice.h"

EthernetDevice::EthernetDevice(NetworkDevice *networkDevice) {
    uint8_t macAddress[6]{0, 0, 0, 0, 0, 0};
    networkDevice->getMacAddress(macAddress);
    this->ethernetAddress = new EthernetAddress(
            macAddress[0],
            macAddress[1],
            macAddress[2],
            macAddress[3],
            macAddress[4],
            macAddress[5]
    );
}

EthernetAddress *EthernetDevice::getEthernetAddress() {
    return ethernetAddress;
}

void EthernetDevice::sendEthernetFrame(EthernetFrame *ethernetFrame) {
    networkDevice->sendPacket(ethernetFrame->getDataAsByteBlock(), ethernetFrame->getLength());
}

uint8_t EthernetDevice::equals(EthernetDevice *ethernetDevice) {
    return this->networkDevice==ethernetDevice->networkDevice;
}

uint8_t EthernetDevice::connectedTo(NetworkDevice *networkDevice) {
    return this->networkDevice==networkDevice;
}
