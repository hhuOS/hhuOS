//
// Created by hannes on 16.05.21.
//

#include "ARPRequest.h"

ARPRequest::ARPRequest(IP4Address *ip4Address) {
    this->ip4Address = ip4Address;
}

uint16_t ARPRequest::getLengthInBytes() {
    return 0;
}

EthernetDataPart::EtherType ARPRequest::getEtherType() {
    return EtherType::ARP;
}

uint8_t ARPRequest::copyDataTo(NetworkByteBlock *byteBlock) {
    //TODO: Implement this one!
    return 1;
}
