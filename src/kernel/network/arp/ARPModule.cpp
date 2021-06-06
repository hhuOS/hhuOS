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
    if (ip4Address == nullptr) {
        return ARP_PROTOCOL_ADDRESS_NULL;
    }
    if (arpTable == nullptr) {
        return ARP_TABLE_NULL;
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
    return 0;
}

void ARPModule::addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) {
    arpTable->add(new ARPEntry(ip4Address, ethernetAddress));
}

EthernetAddress *ARPModule::getBroadcastAddress() const {
    return broadcastAddress;
}
