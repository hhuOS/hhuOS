//
// Created by hannes on 25.05.21.
//

#ifndef HHUOS_ETHERNETDEVICE_H
#define HHUOS_ETHERNETDEVICE_H


#include <device/network/NetworkDevice.h>
#include <kernel/log/Logger.h>
#include "EthernetAddress.h"
#include "EthernetFrame.h"

class EthernetDevice {
private:
    String *identifier;
    NetworkDevice *networkDevice;
    EthernetAddress *ethernetAddress;

    /**
         * A logger to provide information on the kernel log.
         */
    Kernel::Logger &log = Kernel::Logger::get("EthernetDevice");
public:
    EthernetDevice(String *identifier, NetworkDevice *networkDevice);

    [[nodiscard]] String *getIdentifier() const;

    void sendEthernetFrame(EthernetFrame *ethernetFrame);

    uint8_t connectedTo(NetworkDevice *otherDevice);

    bool equals(EthernetDevice *compare);

    String asString();

    EthernetAddress *getAddress();
};


#endif //HHUOS_ETHERNETDEVICE_H
