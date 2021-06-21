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
        Management *systemManagement = nullptr;
    protected:
        Spinlock *sendLock = nullptr;
        uint8_t * sendBuffer= nullptr;
        NetworkDevice *networkDevice = nullptr;
        EthernetAddress *ethernetAddress = nullptr;
        EthernetDeviceIdentifier *identifier = nullptr;

        size_t ETHERNET_MAX_FRAME_SIZE = 0;

        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("EthernetDevice");
    public:

        EthernetDevice(Management *systemManagement, EthernetDeviceIdentifier *identifier,
                       NetworkDevice *networkDevice);

        [[nodiscard]] EthernetDeviceIdentifier *getIdentifier() const;

        virtual uint8_t sendEthernetFrame(EthernetFrame *ethernetFrame);

        bool connectedTo(NetworkDevice *otherDevice);

        bool equals(EthernetDevice *compare);

        String asString();

        EthernetAddress *getAddress();

        bool sameIdentifierAs(EthernetDeviceIdentifier *other);

        virtual ~EthernetDevice();

        EthernetDevice(Management *pManagement, EthernetDeviceIdentifier *pIdentifier, NetworkDevice *pDevice);

        EthernetDevice(Management *pManagement, EthernetDeviceIdentifier *pIdentifier, NetworkDevice *pDevice);
    };
}


#endif //HHUOS_ETHERNETDEVICE_H
