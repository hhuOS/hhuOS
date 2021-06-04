//
// Created by hannes on 02.06.21.
//

#include "ARPMessage.h"

uint8_t ARPMessage::copyTo(NetworkByteBlock *byteBlock) {

    return 0;
}

size_t ARPMessage::getLengthInBytes() {
    return 0;
}

EthernetDataPart::EtherType ARPMessage::getEtherType() {
    return EtherType::ARP;
}

uint8_t ARPMessage::parse(NetworkByteBlock *input) {
    return 1;
}

ARPMessage::ARPMessage(IP4Address *targetProtocolAddress, EthernetAddress *senderHardwareAddress,
                       IP4Address *senderProtocolAddress) {
    this->targetProtocolAddress=targetProtocolAddress;
    this->senderHardwareAddress=senderHardwareAddress;
    this->senderProtocolAddress=senderProtocolAddress;
    header.opCode=1; //This is a request
}
