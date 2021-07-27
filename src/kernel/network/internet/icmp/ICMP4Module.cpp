//
// Created by hannes on 17.05.21.
//

#include <kernel/network/internet/icmp/ICMP4EchoReply.h>
#include <kernel/network/internet/icmp/ICMP4Echo.h>
#include <kernel/event/network/ICMP4SendEvent.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/network/DebugPrintout.h>
#include <kernel/core/System.h>
#include "ICMP4Module.h"

namespace Kernel {

    //Private method!
    uint8_t ICMP4Module::startPingTime(ICMP4Echo *echoMessage) {
        uint16_t sequenceNumber = echoMessage->getSequenceNumber();
        if (sequenceNumber >= ICMP_PING_BUFFER_SIZE) {
            log.error("Too many messages, not taking Ping RTT");
            return 1;
        }
        accessLock->acquire();
        if (pingTimes[sequenceNumber] != 0) {
            log.error("Value not empty, another Ping is still running...");
            accessLock->release();
            return 1;
        }
        pingTimes[sequenceNumber] = timeService->getSystemTime();
        accessLock->release();
        return 0;
    }

    //Private method!
    uint8_t ICMP4Module::stopPingTime(uint32_t *resultTarget, ICMP4EchoReply *echoReplyMessage) {
        uint16_t sequenceNumber = echoReplyMessage->getSequenceNumber();
        if (sequenceNumber >= ICMP_PING_BUFFER_SIZE) {
            log.error("Sequence number too high, not taking Ping RTT");
            return 1;
        }
        accessLock->acquire();
        if (pingTimes[sequenceNumber] == 0) {
            log.error("Value not empty, no start time has been taken...");
            accessLock->release();
            return 1;
        }
        *resultTarget = timeService->getSystemTime() - pingTimes[sequenceNumber];
        pingTimes[sequenceNumber] = 0;
        accessLock->release();
        return 0;
    }

    //Private method!
    uint8_t ICMP4Module::processICMP4Message(IP4Header *ip4Header, NetworkByteBlock *input) {
        uint8_t typeByte = 0;
        input->readOneByteTo(&typeByte);
        //Decrement index by one
        //-> now it points to first message byte again!
        input->decrementIndex();
        switch (ICMP4Message::parseByteAsICMP4MessageType(typeByte)) {
            case ICMP4Message::ICMP4MessageType::ECHO: {
                auto *echoRequest = new ICMP4Echo();
                if (echoRequest->parse(input)) {
                    log.error("Parsing ICMP4Echo failed, discarding");
                    delete echoRequest;
                    return 1;
                }
                if (!echoRequest->checksumIsValid()) {
                    //Checksum is about full message here
                    log.error("ICMP4Echo message corrupted, discarding");
                    delete echoRequest;
                    return 1;
                }
                //create and send reply
                //NOTE: The datagram's attributes will be deleted after sending
                //      -> copy it here!
                auto sourceAddressCopy = new IP4Address(ip4Header->getSourceAddress());
                auto icmp4SendEchoReplyEvent =
                        Util::SmartPointer<Event>(new ICMP4SendEvent(sourceAddressCopy, echoRequest->buildEchoReply()));
                eventBus->publish(icmp4SendEchoReplyEvent);
                delete echoRequest;
                return 0;
            }
            case ICMP4Message::ICMP4MessageType::ECHO_REPLY: {
                auto *echoReply = new ICMP4EchoReply();
                if (echoReply->parse(input)) {
                    log.error("Parsing ICMP4EchoReply failed, discarding");
                    delete echoReply;
                    return 1;
                }
                if (!echoReply->checksumIsValid()) {
                    //Checksum is about full message here
                    log.error("ICMP4EchoReply message corrupted, discarding");
                    delete echoReply;
                    return 1;
                }
#if PRINT_IN_ICMP4ECHOREPLY == 1
                uint32_t timeTotal = 0;
                if (stopPingTime(&timeTotal, echoReply)) {
                    printf("[Taking RTT failed] ");
                }
                printf("ICMP4EchoReply! Source: %s, ID: %d, Sequence: %d, RTT: %d ms\n",
                       (char *) ip4Header->getSourceAddress()->asString(),
                       echoReply->getIdentifier(),
                       echoReply->getSequenceNumber(),
                       timeTotal
                );
#endif
                delete echoReply;
                return 0;
            }
            default:
                log.error("ICMP4MessageType of incoming ICMP4Message not supported, discarding data");
                return 1;
        }
    }

    ICMP4Module::ICMP4Module(EventBus *eventBus) : eventBus(eventBus) {
        this->timeService = System::getService<TimeService>();
        accessLock = new Spinlock();
        for (unsigned int &pingTime : pingTimes) {
            pingTime = 0;
        }
    }

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
                log.error("Destination address was null or checksum calculation failed, discarding message");
                delete icmp4Message;
                return;
            }
            if (icmp4Message->fillChecksumField()) {
                log.error("Checksum calculation failed, discarding message");
                delete icmp4Message;
                return;
            }
#if PRINT_IN_ICMP4ECHOREPLY == 1
            if (icmp4Message->getICMP4MessageType() == ICMP4Message::ICMP4MessageType::ECHO) {
                if (startPingTime((ICMP4Echo *) icmp4Message)) {
                    log.error("Taking RTT when sending Echo Request failed");
                }
            }
#endif
            //Send data to IP4Module for further processing
            auto ip4SendIcmp4MessageEvent =
                    Util::SmartPointer<Event>(new IP4SendEvent(destinationAddress, icmp4Message));
            eventBus->publish(ip4SendIcmp4MessageEvent);

            //we need destinationAddress and icmp4Message in IP4Module
            //-> don't delete anything here
            return;
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
