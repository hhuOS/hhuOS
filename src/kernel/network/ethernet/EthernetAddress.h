//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETADDRESS_H
#define HHUOS_ETHERNETADDRESS_H

#include <device/network/NetworkDevice.h>

class EthernetAddress {
private:
    uint8_t *macAddress = nullptr;
public:
    explicit EthernetAddress(NetworkDevice *networkDevice);

    explicit EthernetAddress(EthernetAddress *other);

    explicit EthernetAddress(const uint8_t *bytes);

    EthernetAddress(
            uint8_t firstByte, uint8_t secondByte, uint8_t thirdByte,
            uint8_t fourthByte, uint8_t fifthByte, uint8_t sixthByte);

    String asString();

    void copyTo(uint8_t *target);

    virtual ~EthernetAddress();

    static EthernetAddress *buildBroadcastAddress();
};


#endif //HHUOS_ETHERNETADDRESS_H
