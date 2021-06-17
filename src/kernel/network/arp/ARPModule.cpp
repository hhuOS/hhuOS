//
// Created by hannes on 15.05.21.
//

#include "ARPModule.h"

namespace Kernel {
    ARPModule::ARPModule() {
        arpTable = new Util::ArrayList<ARPEntry *>();
        broadcastAddress =
                new EthernetAddress(
                        0xff,
                        0xff,
                        0xff,
                        0xff,
                        0xff,
                        0xff
                );
    }

    ARPModule::~ARPModule() {
        delete broadcastAddress;
    }

    EthernetAddress *ARPModule::getBroadcastAddress() const {
        return broadcastAddress;
    }

    uint8_t ARPModule::resolveTo(EthernetAddress **ethernetAddress, IP4Address *ip4Address) {
        if (ip4Address == nullptr) {
            log.error("IP4 address given to ARP module was null, not resolving");
            return 1;
        }
        if (arpTable == nullptr) {
            log.error("ARP table was null, not resolving");
            return 1;
        }
        *ethernetAddress = nullptr;
        for (ARPEntry *current:*arpTable) {
            if (current->matches(ip4Address)) {
                *ethernetAddress = current->getEthernetAddress();
                return 0;
            }
        }
        //If no entry could be found, simply return nullptr as ethernetAddress
        //-> this will cause an ARP resolve
        *ethernetAddress = nullptr;
        return 0;
    }

    void ARPModule::addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) {
        if (ip4Address == nullptr) {
            log.error("Given IP4 address was null, not adding entry");
            return;
        }
        if (ethernetAddress == nullptr) {
            log.error("Given Ethernet address was null, not adding entry");
        }
        if (arpTable == nullptr) {
            log.error("ARP table was null, not adding entry");
            return;
        }
        arpTable->add(new ARPEntry(ip4Address, ethernetAddress));
    }
}