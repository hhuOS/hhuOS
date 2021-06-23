//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ARPMODULE_H
#define HHUOS_ARPMODULE_H

#include <kernel/service/TimeService.h>
#include <kernel/network/ethernet/EthernetDevice.h>
#include <kernel/network/NetworkEventBus.h>
#include "ARPEntry.h"
#include "ARPMessage.h"

namespace Kernel {
    class ARPModule {
    private:
        Spinlock *tableAccessLock = nullptr;
        TimeService *timeService = nullptr;
        NetworkEventBus *eventBus = nullptr;
        EthernetDevice *outDevice = nullptr;
        //HashMap did not work here, possible bug in HashMap implementation?
        Util::ArrayList<ARPEntry *> *arpTable = nullptr;
        /**
             * A logger to provide information on the kernel log.
             */
        Kernel::Logger &log = Kernel::Logger::get("ARPModule");

        bool entryFound(EthernetAddress **ethernetAddress, IP4Address *receiverAddress);

        uint8_t addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress);

    public:

        ARPModule(NetworkEventBus *eventBus, EthernetDevice *outDevice);

        virtual ~ARPModule();

        uint8_t
        resolveTo(EthernetAddress **ethernetAddress, IP4Address *targetProtocolAddress,
                  IP4Address *senderProtocolAddress);

        uint8_t sendRequest(IP4Address *senderProtocolAddress, IP4Address *targetProtocolAddress);

        uint8_t processIncoming(ARPMessage *message);

        String asString();
    };
}

#endif //HHUOS_ARPMODULE_H
