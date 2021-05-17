//
// Created by hannes on 17.05.21.
//

#include "ICMP4EchoReplyEvent.h"

namespace Kernel {

    ICMP4EchoReplyEvent::ICMP4EchoReplyEvent(ICMP4EchoReply *reply) {
        this->reply = reply;
    }

    ICMP4EchoReply *ICMP4EchoReplyEvent::getReply() const {
        return reply;
    }

    String ICMP4EchoReplyEvent::getType() const {
        return TYPE;
    }

}