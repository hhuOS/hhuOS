//
// Created by hannes on 15.05.21.
//

#include "ARPModule.h"

ARPModule::ARPModule() {
    arpTable = new Util::ArrayList<ARPEntry *>();
    broadcastAddress =
            new EthernetAddress(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
}

uint8_t ARPModule::resolveTo(EthernetAddress **ethernetAddress, IP4Address *ip4Address) {
    if (ip4Address == nullptr || arpTable == nullptr) {
        return 1;
    }
    for (ARPEntry *current:*arpTable) {
        if (current->matches(ip4Address)) {
            //We need to copy our ARP entry's address, because the frame's address will be deleted after sending
            *ethernetAddress = new EthernetAddress(current->getEthernetAddress());
            return 0;
        }
    }
    //set destination address to NULL if no ARP entry could be found
    //-> this will cause an ARPRequest
    *ethernetAddress = nullptr;
    return 0;
}

void ARPModule::addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) {
    arpTable->add(new ARPEntry(ip4Address, ethernetAddress));
}

EthernetAddress *ARPModule::getBroadcastAddress() const {
    return broadcastAddress;
}
