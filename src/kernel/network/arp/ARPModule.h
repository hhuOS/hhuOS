//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ARPMODULE_H
#define HHUOS_ARPMODULE_H

#include <lib/util/ArrayList.h>
#include <kernel/log/Logger.h>
#include "ARPEntry.h"

namespace Kernel {
    class ARPModule {
    private:
        //HashMap did not work here, possible bug in HashMap implementation?
        Util::ArrayList<ARPEntry *> *arpTable = nullptr;
        EthernetAddress *broadcastAddress = nullptr;
        /**
             * A logger to provide information on the kernel log.
             */
        Kernel::Logger &log = Kernel::Logger::get("ARPModule");

    public:
        ARPModule();

        virtual ~ARPModule();

        [[nodiscard]] EthernetAddress *getBroadcastAddress() const;

        void addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress);

        uint8_t resolveTo(EthernetAddress **ethernetAddress, IP4Address *ip4Address);
    };
}

#endif //HHUOS_ARPMODULE_H
