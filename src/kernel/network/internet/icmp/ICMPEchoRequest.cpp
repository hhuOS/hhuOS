//
// Created by hannes on 14.05.21.
//

#include "ICMPEchoRequest.h"

ICMPEchoRequest::ICMPEchoRequest() {


}

void *ICMPEchoRequest::getMemoryAddress() {
    return nullptr;
}

uint16_t ICMPEchoRequest::getLength() {
    return 0;
}

IP4ProtocolType ICMPEchoRequest::getIP4ProtocolType() {
    return IP4ProtocolType::ICMP;
}
