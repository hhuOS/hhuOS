//
// Created by hannes on 25.05.21.
//

#ifndef HHUOS_ETHERNETDEVICE_H
#define HHUOS_ETHERNETDEVICE_H


#include <device/network/NetworkDevice.h>
#include "EthernetAddress.h"
#include "EthernetFrame.h"

class EthernetDevice {
private:
    String *identifier;
    NetworkDevice *networkDevice;
    EthernetAddress *ethernetAddress;
public:
    EthernetDevice(String *identifier, NetworkDevice *networkDevice);

    String *getIdentifier() const;

    void sendEthernetFrame(EthernetFrame *ethernetFrame);

    uint8_t connectedTo(NetworkDevice *networkDevice);

    bool equals(EthernetDevice *compare);

    String asString();
};


#endif //HHUOS_ETHERNETDEVICE_H
