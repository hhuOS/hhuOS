//
// Created by hannes on 25.05.21.
//

#ifndef HHUOS_ETHERNETDEVICE_H
#define HHUOS_ETHERNETDEVICE_H


#include <kernel/log/Logger.h>
#include "EthernetAddress.h"
#include "EthernetFrame.h"
#include <kernel/core/Management.h>

namespace Kernel {
    class EthernetDevice {
    private:
        String identifier = "";
        Spinlock *sendLock = nullptr;
        uint8_t *sendBuffer = nullptr;
        void *physicalBufferAddress = nullptr;
        NetworkDevice *networkDevice = nullptr;
        EthernetAddress *ethernetAddress = nullptr;

        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("EthernetDevice");
    public:

        EthernetDevice(const String &identifier, NetworkDevice *networkDevice, uint8_t *sendBuffer,
                       void *physicalBufferAddress);

        virtual ~EthernetDevice();

        bool connectedTo(NetworkDevice *otherDevice);

        bool equals(EthernetDevice *compare);

        bool sameIdentifierAs(const String &other);

        bool isDestinationOf(EthernetHeader *ethernetHeader);

        uint8_t sendEthernetFrame(EthernetFrame *ethernetFrame);

        uint8_t copyAddressTo(uint8_t *target);

        uint8_t copyIdentifierTo(String *target);

        String asString();
    };
}


#endif //HHUOS_ETHERNETDEVICE_H
