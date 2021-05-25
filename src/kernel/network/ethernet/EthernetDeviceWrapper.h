//
// Created by hannes on 25.05.21.
//

#ifndef HHUOS_ETHERNETDEVICEWRAPPER_H
#define HHUOS_ETHERNETDEVICEWRAPPER_H


#include <device/network/NetworkDevice.h>
#include "EthernetDevice.h"
#include "EthernetAddress.h"
#include "EthernetFrame.h"

class EthernetDeviceWrapper : public EthernetDevice {
private:
    NetworkDevice *networkDevice;
    EthernetAddress *ethernetAddress;
public:
    EthernetDeviceWrapper(NetworkDevice *networkDevice);

    EthernetAddress *getEthernetAddress() override;

    void sendEthernetFrame(EthernetFrame *ethernetFrame) override;
};


#endif //HHUOS_ETHERNETDEVICEWRAPPER_H
