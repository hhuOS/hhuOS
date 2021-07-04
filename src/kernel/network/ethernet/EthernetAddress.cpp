//
// Created by hannes on 15.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include "EthernetAddress.h"

EthernetAddress::EthernetAddress(NetworkDevice *networkDevice) {
    if (networkDevice != nullptr) {
        macAddress = new uint8_t[ETH_ADDRESS_LENGTH];
        networkDevice->getMacAddress(macAddress);
    }
}

EthernetAddress::EthernetAddress(
        uint8_t firstByte, uint8_t secondByte, uint8_t thirdByte,
        uint8_t fourthByte, uint8_t fifthByte, uint8_t sixthByte
) {
    macAddress = new uint8_t[ETH_ADDRESS_LENGTH];
    macAddress[0] = firstByte;
    macAddress[1] = secondByte;
    macAddress[2] = thirdByte;
    macAddress[3] = fourthByte;
    macAddress[4] = fifthByte;
    macAddress[5] = sixthByte;
}

EthernetAddress::EthernetAddress(EthernetAddress *other) {
    if (other != nullptr) {
        macAddress = new uint8_t[ETH_ADDRESS_LENGTH];
        other->copyTo(macAddress);
    }
}

EthernetAddress::EthernetAddress(const uint8_t *bytes) {
    if (bytes != nullptr) {
        macAddress = new uint8_t[ETH_ADDRESS_LENGTH];
        for (uint8_t i = 0; i < ETH_ADDRESS_LENGTH; i++) {
            this->macAddress[i] = bytes[i];
        }
    }
}

EthernetAddress::~EthernetAddress() {
    delete macAddress;
}

EthernetAddress *EthernetAddress::buildBroadcastAddress() {
    uint8_t allOnesBytes[ETH_ADDRESS_LENGTH]{0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    return new EthernetAddress(allOnesBytes);
}

String EthernetAddress::asString() {
    if (macAddress == nullptr) {
        return "NULL";
    }
    return String::format("'%02x:%02x:%02x:%02x:%02x:%02x'",
                          macAddress[0], macAddress[1], macAddress[2],
                          macAddress[3], macAddress[4], macAddress[5]
    );
}

uint8_t EthernetAddress::copyTo(uint8_t *target) {
    if (macAddress == nullptr || target == nullptr) {
        return 1;
    }
    for (uint8_t i = 0; i < ETH_ADDRESS_LENGTH; i++) {
        target[i] = this->macAddress[i];
    }
    return 0;
}

bool EthernetAddress::equals(EthernetAddress *otherAddress) {
    if (macAddress == nullptr || otherAddress == nullptr) {
        return false;
    }
    for (uint8_t i = 0; i < ETH_ADDRESS_LENGTH; i++) {
        if (macAddress[i] != otherAddress->macAddress[i]) {
            return false;
        }
    }
    return true;
}


