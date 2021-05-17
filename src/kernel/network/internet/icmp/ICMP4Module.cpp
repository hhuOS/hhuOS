//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/event/network/ICMP4EchoReplyEvent.h>
#include "ICMP4Module.h"
#include "ICMP4MessageType.h"
#include "kernel/network/internet/icmp/messages/ICMP4EchoReply.h"

void Kernel::ICMP4Module::onEvent(const Kernel::Event &event) {
    if(event.getType()==ICMP4ReceiveEvent::TYPE){
        auto dataPart=((ICMP4ReceiveEvent &)event).getIp4DataPart();
        ICMP4MessageType messageType=parseMessageType(dataPart);
        switch (messageType) {
            case ICMP4MessageType::ECHO_REPLY:
                eventBus->publish(
                        Util::SmartPointer<Kernel::Event>(
                                new Kernel::ICMP4EchoReplyEvent(
                                        new ICMP4EchoReply(dataPart)
                                        )
                        )
                );
                break;
            case ICMP4MessageType::DESTINATION_UNREACHABLE:
                break;
            case ICMP4MessageType::SOURCE_QUENCH:
                break;
            case ICMP4MessageType::REDIRECT:
                break;
            case ICMP4MessageType::ECHO:
                break;
            case ICMP4MessageType::TIME_EXCEEDED:
                break;
            case ICMP4MessageType::PARAMETER_PROBLEM:
                break;
            case ICMP4MessageType::TIMESTAMP:
                break;
            case ICMP4MessageType::TIMESTAMP_REPLY:
                break;
            case ICMP4MessageType::INFORMATION_REQUEST:
                break;
            case ICMP4MessageType::INFORMATION_REPLY:
                break;
        }
    }
}

ICMP4MessageType Kernel::ICMP4Module::parseMessageType(IP4DataPart *pPart) {
    //TODO: Implement matching of first by in dataPart to ICMP4MessageType
    return ICMP4MessageType::ECHO_REPLY;
}
