//
// Created by hannes on 14.05.21.
//

#include "ICMP4EchoRequest.h"

ICMP4EchoRequest::ICMP4EchoRequest() {


}

void *ICMP4EchoRequest::getMemoryAddress() {
    return nullptr;
}

uint16_t ICMP4EchoRequest::getLength() {
    return 0;
}

IP4ProtocolType ICMP4EchoRequest::getIP4ProtocolType() {
    return IP4ProtocolType::ICMP;
}
