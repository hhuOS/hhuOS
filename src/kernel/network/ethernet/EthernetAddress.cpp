//
// Created by hannes on 15.05.21.
//

#include "EthernetAddress.h"

EthernetAddress::EthernetAddress(NetworkDevice *networkDevice) {
    networkDevice->getMacAddress(macAddress);
}

EthernetAddress::EthernetAddress(
        uint8_t firstByte, uint8_t secondByte, uint8_t thirdByte,
        uint8_t fourthByte, uint8_t fifthByte, uint8_t sixthByte
) {
    macAddress[0] = firstByte;
    macAddress[1] = secondByte;
    macAddress[2] = thirdByte;
    macAddress[3] = fourthByte;
    macAddress[4] = fifthByte;
    macAddress[5] = sixthByte;
}

String EthernetAddress::asString() {
    return String::format("%02x:%02x:%02x:%02x:%02x:%02x",
                          macAddress[0], macAddress[1], macAddress[2],
                          macAddress[3], macAddress[4], macAddress[5]
    );
}

void EthernetAddress::copyTo(uint8_t *target) {
    memcpy(target, macAddress, MAC_SIZE);
}

void EthernetAddress::copyTo(uint8_t *target, uint8_t *source) {
    memcpy(target, source, MAC_SIZE);
}


