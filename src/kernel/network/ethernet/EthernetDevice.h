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
    NetworkDevice *networkDevice;
    EthernetAddress *ethernetAddress;
public:
    EthernetDevice(NetworkDevice *networkDevice);

    EthernetAddress *getEthernetAddress();

    void sendEthernetFrame(EthernetFrame *ethernetFrame);

    uint8_t equals(EthernetDevice *ethernetDevice);

    uint8_t connectedTo(NetworkDevice *networkDevice);
};


#endif //HHUOS_ETHERNETDEVICE_H
