//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4SendEvent.h>
#include <kernel/network/internet/icmp/messages/ICMP4EchoReply.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <lib/libc/printf.h>
#include "ICMP4Module.h"

namespace Kernel {
    //Private method!
    uint8_t ICMP4Module::processICMP4Message(IP4Header *ip4Header, NetworkByteBlock *input) {
        uint8_t typeByte = 0;
        input->readOneByteTo(&typeByte);
        //Decrement index by one
        //-> now it points to first message byte again!
        input->decrementIndex();
        switch (ICMP4Message::parseByteAsICMP4MessageType(typeByte)) {
            case ICMP4Message::ICMP4MessageType::ECHO_REPLY: {
                auto *echoReply = new ICMP4EchoReply();
                if (echoReply->parse(input)) {
                    log.error("Parsing ICMP4EchoReply failed, discarding");
                    delete echoReply;
                    return 1;
                }
                printf("ICMP4EchoReply received! SourceAddress: %s, Identifier: %d, SequenceNumber: %d\n",
                       ip4Header->getSourceAddress()->asChars(),
                       echoReply->getIdentifier(),
                       echoReply->getSequenceNumber()
                );
                delete echoReply;
                return 0;
            }
            case ICMP4Message::ICMP4MessageType::ECHO: {
                auto *echoRequest = new ICMP4Echo();
                if (echoRequest->parse(input)) {
                    log.error("Parsing ICMP4Echo failed, discarding");
                    delete echoRequest;
                    return 1;
                }
                //create and send reply
                eventBus->publish(
                        new ICMP4SendEvent(
                                //The datagram's attributes will be deleted after sending
                                //-> copy it here!
                                new IP4Address(ip4Header->getSourceAddress()),
                                echoRequest->buildEchoReply()
                        )
                );
                delete echoRequest;
                return 0;
            }
            default:
                log.error("ICMP4MessageType of incoming ICMP4Message not supported, discarding data");
                return 1;
        }
    }

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
                        delete (ICMP4EchoReply *) icmp4Message;
                        return;
                    case ICMP4Message::ICMP4MessageType::ECHO:
                        delete (ICMP4Echo *) icmp4Message;
                        return;
                        //All implemented messages are deleted now
                        //-> we can break here
                        //NOTE: Please add new ICMP4Messages here if implemented!
                    default:
                        break;
                }
            }
            eventBus->publish(
                    new IP4SendEvent(
                            new IP4Datagram(destinationAddress, icmp4Message)
                    )
            );
        }
        if ((event.getType() == ICMP4ReceiveEvent::TYPE)) {
            auto *ip4Header = ((ICMP4ReceiveEvent &) event).getIP4Header();
            auto *input = ((ICMP4ReceiveEvent &) event).getInput();
            if (input == nullptr || ip4Header == nullptr) {
                log.error("Incoming IP4Header or input was null, discarding data");
                delete ip4Header;
                delete input;
                return;
            }
            if (processICMP4Message(ip4Header, input)) {
                log.error("Could not process ICMP4Message, see syslog for more details");
            }
            //Processing finally done, cleanup
            delete ip4Header;
            delete input;
            return;
        }
    }
}
