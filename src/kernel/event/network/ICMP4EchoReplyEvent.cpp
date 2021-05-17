//
// Created by hannes on 17.05.21.
//

#include "ICMP4EchoReplyEvent.h"

Kernel::ICMP4EchoReplyEvent::ICMP4EchoReplyEvent(ICMP4EchoReply *reply) {
    this->reply=reply;
}

ICMP4EchoReply *Kernel::ICMP4EchoReplyEvent::getReply() const {
    return reply;
}

String Kernel::ICMP4EchoReplyEvent::getType() const {
    return TYPE;
}
