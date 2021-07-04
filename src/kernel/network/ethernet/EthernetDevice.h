//
// Created by hannes on 25.05.21.
//

#ifndef HHUOS_ETHERNETDEVICE_H
#define HHUOS_ETHERNETDEVICE_H


#include <kernel/log/Logger.h>
#include "EthernetAddress.h"
#include "EthernetFrame.h"
#include "EthernetDeviceIdentifier.h"
#include <kernel/core/Management.h>

namespace Kernel {
    class EthernetDevice {
    private:
        Spinlock *sendLock = nullptr;
        uint8_t *sendBuffer = nullptr;
        void *physicalBufferAddress = nullptr;
        NetworkDevice *networkDevice = nullptr;
        EthernetAddress *ethernetAddress = nullptr;
        EthernetDeviceIdentifier *identifier = nullptr;

        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("EthernetDevice");
    public:

        EthernetDevice(uint8_t *sendBuffer, void *physicalBufferAddress, EthernetDeviceIdentifier *identifier,
                       NetworkDevice *networkDevice);

        EthernetDevice(uint8_t *sendBuffer, EthernetDeviceIdentifier *identifier, NetworkDevice *networkDevice);

        [[nodiscard]] uint8_t *getSendBuffer() const;

        [[nodiscard]] EthernetDeviceIdentifier *getIdentifier() const;

        virtual uint8_t sendEthernetFrame(EthernetFrame *ethernetFrame);

        bool connectedTo(NetworkDevice *otherDevice);

        bool equals(EthernetDevice *compare);

        String asString();

        bool sameIdentifierAs(EthernetDeviceIdentifier *other);

        virtual ~EthernetDevice();

        [[nodiscard]] void *getPhysicalBufferAddress() const;

        bool isDestinationOf(EthernetHeader *ethernetHeader);

        uint8_t copyAddressTo(uint8_t* target);
    };
}


#endif //HHUOS_ETHERNETDEVICE_H
