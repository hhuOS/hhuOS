//
// Created by hannes on 14.05.21.
//

#include "ICMP4Echo.h"

ICMP4Echo::ICMP4Echo() {


}

void *ICMP4Echo::getMemoryAddress() {
    return nullptr;
}

uint16_t ICMP4Echo::getLength() {
    return 0;
}

IP4ProtocolType ICMP4Echo::getIP4ProtocolType() {
    return IP4ProtocolType::ICMP;
}
