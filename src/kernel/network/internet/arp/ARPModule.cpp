//
// Created by hannes on 15.05.21.
//

#include "ARPModule.h"

EthernetAddress *ARPModule::resolveIP4(IP4Address *ip4Address) {
    if (arpTable->containsKey(ip4Address)) {
        return arpTable->get(ip4Address);
    }
    return nullptr;
}

ARPModule::ARPModule() {
    arpTable=new Util::HashMap<IP4Address *,EthernetAddress *>();
}

ARPModule::~ARPModule() {
    delete arpTable;
}

void ARPModule::addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) {
    arpTable->put(ip4Address,ethernetAddress);
}
