//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ARPMODULE_H
#define HHUOS_ARPMODULE_H

#include <lib/util/HashMap.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/ethernet/EthernetAddress.h>
#include <kernel/network/ethernet/EthernetFrame.h>
#include <kernel/network/internet/IP4Datagram.h>
#include <kernel/log/Logger.h>
#include "ARPEntry.h"

namespace Kernel{
        class ARPModule {
            private:
            //HashMap did not work here, possible bug in HashMap implementation?
            Util::ArrayList<ARPEntry *> *arpTable;
            EthernetAddress *broadcastAddress;
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
