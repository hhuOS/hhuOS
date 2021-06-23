//
// Created by hannes on 29.05.21.
//

#include "ARPEntry.h"

ARPEntry::ARPEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) : ip4Address(ip4Address),
                                                                               ethernetAddress(ethernetAddress) {}

ARPEntry::~ARPEntry() {
    delete ethernetAddress;
    delete ip4Address;
}

bool ARPEntry::matches(IP4Address *otherAddress) {
    return this->ip4Address->equals(otherAddress);
}

EthernetAddress *ARPEntry::getEthernetAddress() {
    return ethernetAddress;
}

void ARPEntry::setEthernetAddress(EthernetAddress *otherAddress) {
    this->ethernetAddress = otherAddress;
}

String ARPEntry::asString() {
    return "'" + ip4Address->asString() + "' => '" + ethernetAddress->asString() + "'";
}
