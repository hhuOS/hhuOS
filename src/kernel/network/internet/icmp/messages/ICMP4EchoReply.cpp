//
// Created by hannes on 17.05.21.
//

#include "ICMP4EchoReply.h"

ICMP4EchoReply::ICMP4EchoReply() {

}

ICMP4EchoReply::ICMP4EchoReply(IP4DataPart *dataPart) {
    //TODO: Implement parsing from input
}

void *ICMP4EchoReply::getMemoryAddress() {
    return nullptr;
}

uint16_t ICMP4EchoReply::getLength() {
    return 0;
}

IP4ProtocolType ICMP4EchoReply::getIP4ProtocolType() {
    return IP4ProtocolType::ICMP;
}
