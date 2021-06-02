//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <lib/libc/printf.h>
#include "ICMP4Module.h"

Kernel::ICMP4Module::ICMP4Module(NetworkEventBus *eventBus) : eventBus(eventBus) {}

void Kernel::ICMP4Module::onEvent(const Kernel::Event &event) {
    if ((event.getType() == ICMP4ReceiveEvent::TYPE)) {
        auto receiveEvent = (ICMP4ReceiveEvent &) event;
        auto genericIcmp4Message = receiveEvent.getGenericIcmp4Message();
        if (genericIcmp4Message->getLengthInBytes() == 0) {
            log.error("Given data was empty! Ignoring...");
            delete genericIcmp4Message;
            return;
        }
        if (genericIcmp4Message->parseInput()) {
            log.error("Parsing of incoming generic ICMP4Message failed, discarding");
            delete genericIcmp4Message;
            return;
        }
        //NOTE: No message should read its 'type' byte internally!
        //-> this byte already is in GenericICMP4Message!
        //Our NetworkByteBlock would read one byte too much and fail...
        //But this is no problem here, because all 'type' values are constant per definition
        //-> check out header structs in our ICMP4Messages for default values
        switch (genericIcmp4Message->getICMP4MessageType()) {
            case ICMP4Message::ICMP4MessageType::ECHO_REPLY: {
                auto *echoReply = genericIcmp4Message->buildICMP4EchoReplyWithInput();
                if (echoReply->parseInput()) {
                    log.error("Parsing of incoming ICMP4 echo reply failed, discarding");
                    delete echoReply;
                    return;
                }
                uint8_t addressBytes[4]{0, 0, 0, 0};
                echoReply->getSourceAddress()->copyTo(addressBytes);

                printf("Echo reply from %d.%d.%d.%d received! Identifier: %d, SequenceNumber: %d",
                       addressBytes[0], addressBytes[1], addressBytes[2], addressBytes[3],
                       echoReply->getIdentifier(), echoReply->getSequenceNumber()
                );
                delete echoReply;
                return;
            }
            case ICMP4Message::ICMP4MessageType::DESTINATION_UNREACHABLE:
                //TODO: Notify application
                return;
            case ICMP4Message::ICMP4MessageType::ECHO: {
                auto *echoRequest = genericIcmp4Message->buildICMP4EchoWithInput();
                if (echoRequest->parseInput()) {
                    log.error("Parsing of incoming ICMP4 echo failed, discarding");
                    delete echoRequest;
                    return;
                }
                //Save destination address or we will loose it when we delete echoRequest later!
                uint8_t addressBytes[4]{0,0,0,0};
                echoRequest->getSourceAddress()->copyTo(addressBytes);

                eventBus->publish(
                        new Kernel::IP4SendEvent(
                                new IP4Datagram(
                                        new IP4Address(addressBytes),
                                        echoRequest->buildEchoReply()
                                        )
                                )
                        );
                delete echoRequest;
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
