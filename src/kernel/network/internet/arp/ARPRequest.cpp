//
// Created by hannes on 16.05.21.
//

#include "ARPRequest.h"

ARPRequest::ARPRequest(IP4Address *ip4Address) {
    this->ip4Address=ip4Address;
}

uint16_t ARPRequest::getLengthInBytes() {
    return 0;
}

EtherType ARPRequest::getEtherType() {
    return EtherType::ARP;
}

void *ARPRequest::getMemoryAddress() {
    return nullptr;
}
