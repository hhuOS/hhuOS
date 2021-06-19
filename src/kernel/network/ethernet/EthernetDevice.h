//
// Created by hannes on 25.05.21.
//

#ifndef HHUOS_ETHERNETDEVICE_H
#define HHUOS_ETHERNETDEVICE_H


#include <kernel/log/Logger.h>
#include "EthernetAddress.h"
#include "EthernetFrame.h"

namespace Kernel {
    class EthernetDevice {
    private:
        String *identifier = nullptr;
        NetworkDevice *networkDevice = nullptr;
        EthernetAddress *ethernetAddress = nullptr;

        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("EthernetDevice");

    public:
        EthernetDevice(String *identifier, NetworkDevice *networkDevice);

        [[nodiscard]] String *getIdentifier() const;

        uint8_t sendEthernetFrame(EthernetFrame *ethernetFrame);

        bool connectedTo(NetworkDevice *otherDevice);

        bool equals(EthernetDevice *compare);

        String asString();

        EthernetAddress *getAddress();
    };
}


#endif //HHUOS_ETHERNETDEVICE_H
