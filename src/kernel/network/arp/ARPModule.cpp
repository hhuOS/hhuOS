//
// Created by hannes on 15.05.21.
//

#include "ARPModule.h"

namespace Kernel {
    ARPModule::ARPModule() {
        arpTable = new Util::ArrayList<ARPEntry *>();
        broadcastAddress =
                new EthernetAddress(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
    }

    uint8_t ARPModule::resolveTo(EthernetAddress **ethernetAddress, IP4Address *ip4Address) {
        if (ip4Address == nullptr) {
            log.error("IP4 address given to ARP module was null, discarding datagram");
            return 1;
        }
        if (arpTable == nullptr) {
            log.error("Table in ARP module was null, discarding datagram");
            return 1;
        }
        *ethernetAddress = nullptr;
        for (ARPEntry *current:*arpTable) {
            if (current->matches(ip4Address)) {
                //We need to copy our ARP entry's address, because the frame's address will be deleted after sending
                *ethernetAddress = new EthernetAddress(current->getEthernetAddress());
                return 0;
            }
        }
        //If no entry could be found, simply return nullptr as ethernetAddress
        //-> this will cause an ARP resolve
        return 1;
    }

    void ARPModule::addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) {
        arpTable->add(new ARPEntry(ip4Address, ethernetAddress));
    }

    EthernetAddress *ARPModule::getBroadcastAddress() const {
        return broadcastAddress;
    }

    ARPModule::~ARPModule() {
        delete broadcastAddress;
    }
}