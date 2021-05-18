//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include "ICMP4Module.h"
#include "ICMP4MessageType.h"
#include "kernel/network/internet/icmp/messages/ICMP4EchoReply.h"

void Kernel::ICMP4Module::onEvent(const Kernel::Event &event) {
    if (event.getType() == ICMP4ReceiveEvent::TYPE) {
        auto receiveEvent = (ICMP4ReceiveEvent &) event;
        auto dataPart = receiveEvent.getIp4DataPart();
        if (dataPart->getLength() == 0) {
            log.error("Given IP4DataPart was empty! Ignoring...");
            return;
        }
        uint8_t firstByte = static_cast<uint8_t *>(dataPart->getMemoryAddress())[0];
        ICMP4MessageType messageType = ICMP4Message::parseMessageType(firstByte);
        switch (messageType) {
            case ICMP4MessageType::ECHO_REPLY:
                //TODO: Notify application
                return;
            case ICMP4MessageType::DESTINATION_UNREACHABLE:
                //TODO: Notify application
                return;
            case ICMP4MessageType::ECHO: {
                auto *echoRequest=new ICMP4Echo(dataPart);
                auto *echoReply=new ICMP4EchoReply(
                        echoRequest->getIdentifier(),
                        echoRequest->getSequenceNumber()
                );
                auto *outDatagram=new IP4Datagram(
                        receiveEvent.getSourceAddress(),
                        echoReply
                        );
                eventBus->publish(
                        Util::SmartPointer<Kernel::Event>(
                                new Kernel::IP4SendEvent(outDatagram)
                        )
                );
                return;
            }
            case ICMP4MessageType::TIME_EXCEEDED:
                //TODO: Notify application
                return;
            default: return;
        }
    }
}
