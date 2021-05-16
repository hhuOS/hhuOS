//
// Created by hannes on 16.05.21.
//

#include <kernel/network/ethernet/EthernetFrame.h>
#include "ARPRequest.h"

void *ARPRequest::getData() {
    return nullptr;
}

uint16_t ARPRequest::getLength() {
    return 0;
}

ARPRequest::ARPRequest(IP4Address *ip4Address) {
    this->ip4Address=ip4Address;
}

uint16_t ARPRequest::getEtherType() {
    return EthernetFrame::ETHERTYPE_ARP;
}
