//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include "ICMP4Module.h"
#include "ICMP4MessageType.h"
#include "kernel/network/internet/icmp/messages/ICMP4EchoReply.h"

void Kernel::ICMP4Module::onEvent(const Kernel::Event &event) {
    if(event.getType()==ICMP4ReceiveEvent::TYPE){
        auto dataPart=((ICMP4ReceiveEvent &)event).getIp4DataPart();
        if(dataPart->getLength()==0){
            log.error("Given IP4DataPart was empty! Ignoring...");
            return;
        }
        uint8_t firstByte = static_cast<uint8_t *>(dataPart->getMemoryAddress())[0];
        ICMP4MessageType messageType=ICMP4Message::parseMessageType(firstByte);
        switch (messageType) {
            //TODO: Do magic
            case ICMP4MessageType::ECHO_REPLY:
                return;
            case ICMP4MessageType::DESTINATION_UNREACHABLE:
                return;
            case ICMP4MessageType::SOURCE_QUENCH:
                return;
            case ICMP4MessageType::REDIRECT:
                return;
            case ICMP4MessageType::ECHO:
                return;
            case ICMP4MessageType::TIME_EXCEEDED:
                return;
            case ICMP4MessageType::PARAMETER_PROBLEM:
                return;
            case ICMP4MessageType::TIMESTAMP:
                return;
            case ICMP4MessageType::TIMESTAMP_REPLY:
                return;
            case ICMP4MessageType::INFORMATION_REQUEST:
                return;
            case ICMP4MessageType::INFORMATION_REPLY:
                return;
            case ICMP4MessageType::INVALID_MESSAGE_TYPE:
                return;
        }
    }
}
