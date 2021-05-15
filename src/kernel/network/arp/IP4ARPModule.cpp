//
// Created by hannes on 15.05.21.
//

#include "IP4ARPModule.h"

EthernetAddress *IP4ARPModule::resolveIP4(IP4Address *ip4Address) {
    if (this->arpTable.containsKey(ip4Address)) {
        return arpTable.get(ip4Address);
    }
    sendARPRequest(ip4Address);
    return nullptr;
}

void IP4ARPModule::sendARPRequest(IP4Address *ip4Address) {
    //interface->send(new ARPRequest(ip4Address))
}
