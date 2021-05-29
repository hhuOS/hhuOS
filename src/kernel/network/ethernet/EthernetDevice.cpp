//
// Created by hannes on 26.05.21.
//

#include "EthernetDevice.h"

EthernetDevice::EthernetDevice(String *identifier, NetworkDevice *networkDevice) {
    this->identifier = identifier;
    this->networkDevice = networkDevice;
    this->ethernetAddress = new EthernetAddress(networkDevice);
}

String *EthernetDevice::getIdentifier() const {
    return identifier;
}

void EthernetDevice::sendEthernetFrame(EthernetFrame *ethernetFrame) {
    ethernetFrame->setSourceAddress(this->ethernetAddress);
    this->networkDevice->sendPacket(
            ethernetFrame->getDataAsByteBlock(),
            ethernetFrame->getLength()
            );
}

uint8_t EthernetDevice::connectedTo(NetworkDevice *networkDevice) {
    return this->networkDevice == networkDevice;
}

String EthernetDevice::asString() {
    return "\nID: " + *identifier + ",\nMAC: " + ethernetAddress->asString();
}

bool EthernetDevice::equals(EthernetDevice *compare) {
    return this->networkDevice == compare->networkDevice;
}

EthernetAddress *EthernetDevice::getAddress() {
    return ethernetAddress;
}
