//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include "ICMP4Module.h"

Kernel::ICMP4Module::ICMP4Module(NetworkEventBus *eventBus) : eventBus(eventBus) {}

void Kernel::ICMP4Module::onEvent(const Kernel::Event &event) {
    if (event.getType() == ICMP4ReceiveEvent::TYPE) {
        auto receiveEvent = (ICMP4ReceiveEvent &) event;
        auto genericIcmp4Message = receiveEvent.getGenericIcmp4Message();
        if (genericIcmp4Message->getLengthInBytes() == 0) {
            log.error("Given IP4DataPart was empty! Ignoring...");
            return;
        }
        if (genericIcmp4Message->parseInput()) {
            log.error("Parsing of incoming generic ICMP4Message failed, discarding");
            delete genericIcmp4Message;
            return;
        }
        switch (genericIcmp4Message->getICMP4MessageType()) {
            case ICMP4Message::ICMP4MessageType::ECHO_REPLY:
                //TODO: Notify application
                return;
            case ICMP4Message::ICMP4MessageType::DESTINATION_UNREACHABLE:
                //TODO: Notify application
                return;
            case ICMP4Message::ICMP4MessageType::ECHO: {
//                auto *echoRequest = new ICMP4Echo(icmp4message);
//                auto *echoReply = new ICMP4EchoReply(
//                        echoRequest->getIdentifier(),
//                        echoRequest->getSequenceNumber()
//                );
//                auto *outDatagram = new IP4Datagram(
//                        receiveEvent.getSourceAddress(),
//                        echoReply
//                );
//                eventBus->publish(
//                        Util::SmartPointer<Kernel::Event>(
//                                new Kernel::IP4SendEvent(outDatagram)
//                        )
//                );
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
