//
// Created by hannes on 15.05.21.
//

#include "ARPModule.h"

ARPModule::ARPModule() {
    arpTable = new Util::ArrayList<ARPEntry *>();
    broadcastAddress =
            new EthernetAddress(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
}

EthernetAddress *ARPModule::resolveIP4(IP4Address *ip4Address) {
    for (ARPEntry *current:*arpTable) {
        if (current->matches(ip4Address)) {
            return current->getEthernetAddress();
        }
    }
    return nullptr;
}

void ARPModule::addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) {
    arpTable->add(new ARPEntry(ip4Address, ethernetAddress));
}

EthernetAddress *ARPModule::getBroadcastAddress() const {
    return broadcastAddress;
}
