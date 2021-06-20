//
// Created by hannes on 25.05.21.
//

#ifndef HHUOS_ETHERNETDEVICE_H
#define HHUOS_ETHERNETDEVICE_H


#include <kernel/log/Logger.h>
#include "EthernetAddress.h"
#include "EthernetFrame.h"
#include "EthernetDeviceIdentifier.h"

namespace Kernel {
    class EthernetDevice {
    private:
        EthernetDeviceIdentifier *identifier = nullptr;
        NetworkDevice *networkDevice = nullptr;
        EthernetAddress *ethernetAddress = nullptr;

        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("EthernetDevice");

    public:
        EthernetDevice(EthernetDeviceIdentifier *identifier, NetworkDevice *networkDevice);

        [[nodiscard]] EthernetDeviceIdentifier *getIdentifier() const;

        uint8_t sendEthernetFrame(EthernetFrame *ethernetFrame);

        bool connectedTo(NetworkDevice *otherDevice);

        bool equals(EthernetDevice *compare);

        String asString();

        EthernetAddress *getAddress();

        bool sameIdentifierAs(EthernetDeviceIdentifier *other);
    };
}


#endif //HHUOS_ETHERNETDEVICE_H
