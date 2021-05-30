//
// Created by hannes on 29.05.21.
//

#include "ARPEntry.h"

ARPEntry::ARPEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) : ip4Address(ip4Address),
                                                                               ethernetAddress(ethernetAddress) {}

bool ARPEntry::matches(IP4Address *ip4Address) {
    return this->ip4Address->equals(ip4Address);
}

EthernetAddress *ARPEntry::getEthernetAddress() {
    return ethernetAddress;
}
