//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include "ICMP4Module.h"
#include "kernel/network/internet/icmp/messages/ICMP4EchoReply.h"

void Kernel::ICMP4Module::onEvent(const Kernel::Event &event) {
    if (event.getType() == ICMP4ReceiveEvent::TYPE) {
        auto receiveEvent = (ICMP4ReceiveEvent &) event;
        auto icmp4message = (ICMP4Message *) receiveEvent.getIp4DataPart();
        if (icmp4message->getLengthInBytes() == 0) {
            log.error("Given IP4DataPart was empty! Ignoring...");
            return;
        }

        switch (icmp4message->getICMP4MessageTypeFromFirstByte()) {
            case ICMP4Message::ICMP4MessageType::ECHO_REPLY:
                //TODO: Notify application
                return;
            case ICMP4Message::ICMP4MessageType::DESTINATION_UNREACHABLE:
                //TODO: Notify application
                return;
            case ICMP4Message::ICMP4MessageType::ECHO: {
                auto *echoRequest = new ICMP4Echo(icmp4message);
                auto *echoReply = new ICMP4EchoReply(
                        echoRequest->getIdentifier(),
                        echoRequest->getSequenceNumber()
                );
                auto *outDatagram = new IP4Datagram(
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
            case ICMP4Message::ICMP4MessageType::TIME_EXCEEDED:
                //TODO: Notify application
                return;
                //Just ignore input if message type not implemented
            default:
                return;
        }
    }
}
