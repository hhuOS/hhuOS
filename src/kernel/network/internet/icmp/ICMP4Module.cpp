//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/event/network/ICMP4EchoReplyEvent.h>
#include "ICMP4Module.h"
#include "ICMP4MessageType.h"
#include "ICMP4EchoReply.h"

void Kernel::ICMP4Module::onEvent(const Kernel::Event &event) {
    if(event.getType()==ICMP4ReceiveEvent::TYPE){
        auto dataPart=((ICMP4ReceiveEvent &)event).getIp4DataPart();
        ICMP4MessageType messageType=parseMessageType(dataPart);
        switch (messageType) {
            case ICMP4MessageType::ECHO_REPLY:
                auto *reply=new ICMP4EchoReply(dataPart);
                eventBus->publish(
                        Util::SmartPointer<Kernel::Event>(
                                new Kernel::ICMP4EchoReplyEvent(reply)
                        )
                );
                break;
        }
    }
}

ICMP4MessageType Kernel::ICMP4Module::parseMessageType(IP4DataPart *pPart) {
    //TODO: Implement matching of first by in dataPart to ICMP4MessageType
    return ICMP4MessageType::ECHO_REPLY;
}
