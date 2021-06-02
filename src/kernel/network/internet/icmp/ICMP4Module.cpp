//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
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
            case ICMP4Message::ICMP4MessageType::ECHO_REPLY:
                //TODO: Notify application
                return;
            case ICMP4Message::ICMP4MessageType::DESTINATION_UNREACHABLE:
                //TODO: Notify application
                return;
            case ICMP4Message::ICMP4MessageType::ECHO: {
                auto *echoRequest = genericIcmp4Message->buildICMP4EchoWithInput();
                if (echoRequest->parseInput()) {
                    log.error("Parsing of incoming ICMP4 echo failed, discarding");
                    delete echoRequest;
                    return;
                }:w

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
