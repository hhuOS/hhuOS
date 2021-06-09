//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <lib/libc/printf.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include <kernel/event/network/ICMP4SendEvent.h>
#include <kernel/network/internet/icmp/messages/ICMP4TimeExceeded.h>
#include <kernel/network/internet/icmp/messages/ICMP4DestinationUnreachable.h>
#include "ICMP4Module.h"

namespace Kernel {
    ICMP4Module::ICMP4Module(NetworkEventBus *eventBus) : eventBus(eventBus) {}

    void ICMP4Module::onEvent(const Event &event) {
        if ((event.getType() == ICMP4SendEvent::TYPE)) {
            auto *destinationAddress = ((ICMP4SendEvent &) event).getDestinationAddress();
            auto *icmp4Message = ((ICMP4SendEvent &) event).getIcmp4Message();
            if (icmp4Message == nullptr) {
                log.error("Outgoing ICMP4 message was null, ignoring");
                delete destinationAddress;
                return;
            }
            if (destinationAddress == nullptr) {
                log.error("Destination address was null, discarding message");
                switch (icmp4Message->getICMP4MessageType()) {
                    case ICMP4Message::ICMP4MessageType::ECHO_REPLY:
                        delete (ICMP4EchoReply *)icmp4Message;
                        return;
                    case ICMP4Message::ICMP4MessageType::DESTINATION_UNREACHABLE:
                        delete (ICMP4DestinationUnreachable *)icmp4Message;
                        return;
                    case ICMP4Message::ICMP4MessageType::ECHO:
                        delete (ICMP4Echo *)icmp4Message;
                        return;
                    case ICMP4Message::ICMP4MessageType::TIME_EXCEEDED:
                        delete (ICMP4TimeExceeded *)icmp4Message;
                        return;
                        //All implemented messages are deleted now
                        //-> we can break here
                        //NOTE: Please add new ICMP4Messages here if implemented!
                    default: break;
                }
            }
            eventBus->publish(
                    new IP4SendEvent(
                            new IP4Datagram(destinationAddress, icmp4Message)
                    )
            );
            //Address has been copied inside constructor, can be deleted now
            delete destinationAddress;
        }
        if ((event.getType() == ICMP4ReceiveEvent::TYPE)) {
            auto *input = ((ICMP4ReceiveEvent &) event).getInput();
            auto *ip4Datagram = new IP4Datagram();
            if(
                    ip4Datagram->parseHeader(input) ||
                    ip4Datagram->copyHeader(&headerInfo, sizeof headerInfo)
            ){
                log.error("Parsing IP4 information failed, discarding");
                delete ip4Datagram;
                delete input;
                return;
            }
            delete ip4Datagram;

            uint8_t typeByte = 0;
            input->read(&typeByte);
            //Decrement index by one
            //-> now it points to first message byte again!
            input->decreaseIndex(1);
            switch (ICMP4Message::parseByteAsICMP4MessageType(typeByte)) {
                case ICMP4Message::ICMP4MessageType::ECHO_REPLY: {
                    auto *sourceAddress = new IP4Address(headerInfo.sourceAddress);
                    auto *echoReply = new ICMP4EchoReply(sourceAddress);
                    if(echoReply->parseHeader(input)){
                        log.error("Parsing ICMP4EchoReply failed, discarding");
                        delete echoReply;
                        delete input;
                        return;
                    }

                    echoReply->printAttributes();
                    //We are done here, cleanup memory
                    delete sourceAddress;
                    delete echoReply;
                    delete input;
                    return;
                }
                case ICMP4Message::ICMP4MessageType::DESTINATION_UNREACHABLE: {
                    auto *destinationUnreachable = new ICMP4DestinationUnreachable(0);
                    if (destinationUnreachable->parseHeader(input) || destinationUnreachable->parseDataBytes(input)) {
                        log.error("Parsing ICMP4DestinationUnreachable failed, discarding");
                        delete destinationUnreachable;
                        delete input;
                        return;
                    }

                    //destinationUnreachable->collectDatagramAttributes(nullptr); //TODO: Implement printing!
                    delete destinationUnreachable;
                    delete input;
                    return;
                }
                case ICMP4Message::ICMP4MessageType::ECHO: {
                    auto *echoRequest = new ICMP4Echo();
                    if(echoRequest->parseHeader(input)){
                        log.error("Parsing ICMP4Echo failed, discarding");
                        delete echoRequest;
                        delete input;
                        return;
                    }

                    //create and send reply
                    eventBus->publish(
                            new IP4SendEvent(
                                    new IP4Datagram(
                                            new IP4Address(headerInfo.sourceAddress),
                                            echoRequest->buildEchoReply()
                                    )
                            )
                    );
                    //We are done here, cleanup memory
                    delete echoRequest;
                    delete input;
                    return;
                }
                case ICMP4Message::ICMP4MessageType::TIME_EXCEEDED: {
                    auto *timeExceeded = new ICMP4TimeExceeded();
                    if (timeExceeded->parseHeader(input)) {
                        log.error("Parsing ICMP4TimeExceeded failed, discarding");
                        delete timeExceeded;
                        delete input;
                        return;
                    }
                    log.info("Received TIME_EXCEEDED");
                    delete timeExceeded;
                    delete input;
                    return;
                }
                default:
                    log.info("ICMP4MessageType of incoming ICMP4Message not supported, discarding data");
                    //No message parsed here, we just need to delete incoming input
                    delete input;
                    return;
            }
        }
    }
}
