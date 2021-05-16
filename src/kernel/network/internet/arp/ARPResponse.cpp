//
// Created by hannes on 16.05.21.
//

#include "ARPResponse.h"

ARPResponse::ARPResponse(EthernetDataPart *dataPart) {
    //TODO: Implement parsing from incoming data
    this->ethernetAddress = nullptr;
    this->ip4Address = nullptr;
}

IP4Address *ARPResponse::getIp4Address() const {
    return ip4Address;
}

EthernetAddress *ARPResponse::getEthernetAddress() const {
    return ethernetAddress;
}
