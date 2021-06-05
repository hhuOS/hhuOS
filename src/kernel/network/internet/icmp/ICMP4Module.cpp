//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <lib/libc/printf.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include "ICMP4Module.h"

Kernel::ICMP4Module::ICMP4Module(NetworkEventBus *eventBus) : eventBus(eventBus) {}

void Kernel::ICMP4Module::onEvent(const Kernel::Event &event) {
    if ((event.getType() == ICMP4ReceiveEvent::TYPE)) {
        auto icmp4Message = ((ICMP4ReceiveEvent &) event).getIcmp4Message();
        //NOTE: No message should read its 'type' byte internally!
        //-> this byte already is in GenericICMP4Message!
        //Our NetworkByteBlock would read one byte too much and fail...
        //This is no problem here because all 'type' values are constant per definition
        //-> check out header structs in ICMP4Messages for default values
        switch (icmp4Message->getICMP4MessageType()) {
            case ICMP4Message::ICMP4MessageType::ECHO_REPLY: {
                auto *echoReply = (ICMP4EchoReply *)icmp4Message;

                uint8_t addressBytes[4]{0, 0, 0, 0};
                echoReply->getSourceAddress()->copyTo(addressBytes);

                printf("Echo reply from %d.%d.%d.%d received! Identifier: %d, SequenceNumber: %d",
                       addressBytes[0], addressBytes[1], addressBytes[2], addressBytes[3],
                       echoReply->getIdentifier(), echoReply->getSequenceNumber()
                );
                //We are done here, cleanup memory
                delete echoReply;
                return;
            }
            case ICMP4Message::ICMP4MessageType::DESTINATION_UNREACHABLE:
                //TODO: Notify application
                return;
            case ICMP4Message::ICMP4MessageType::ECHO: {
                auto *echoRequest = (ICMP4Echo *)icmp4Message;

                //create and send reply
                eventBus->publish(
                        new Kernel::IP4SendEvent(
                                new IP4Datagram(
                                        echoRequest->getSourceAddress(),
                                        echoRequest->buildEchoReply()
                                )
                        )
                );
                //We are done here, cleanup memory
                delete echoRequest;
                return;
            }
            case ICMP4Message::ICMP4MessageType::TIME_EXCEEDED:
                //TODO: Notify application
                return;
            default:
                log.info("ICMP4MessageType of incoming ICMP4Message not supported, discarding");
                delete icmp4Message;
                return;
        }
    }
}
