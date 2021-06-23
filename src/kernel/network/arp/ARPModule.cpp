//
// Created by hannes on 15.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/service/TimeService.h>
#include "ARPModule.h"

namespace Kernel {
    //Private method!
    //Why private? We might delete given addresses at any time or in destructor at least
    //-> very dangerous if public, we MUST make sure that only new [Address]() is given here
    uint8_t ARPModule::addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) {
        if (ip4Address == nullptr) {
            log.error("Given IP4 address was null, not adding entry");
            return 1;
        }
        if (ethernetAddress == nullptr) {
            log.error("Given Ethernet address was null, not adding entry");
            return 1;
        }
        if (arpTable == nullptr || tableAccessLock == nullptr) {
            log.error("ARP table or access lock was null, not adding entry");
            return 1;
        }
        tableAccessLock->acquire();
        for (ARPEntry *current:*arpTable) {
            //Update existing entry instead of creating a new one if address already known
            if (current->matches(ip4Address)) {
                delete current->getEthernetAddress();
                current->setEthernetAddress(ethernetAddress);
                tableAccessLock->release();
                return 0;
            }
        }
        arpTable->add(new ARPEntry(ip4Address, ethernetAddress));
        tableAccessLock->release();
        return 0;
    }

    //Private method!
    bool ARPModule::entryFound(EthernetAddress **ethernetAddress, IP4Address *receiverAddress) {
        if (arpTable == nullptr || tableAccessLock == nullptr) {
            return false;
        }
        tableAccessLock->acquire();
        *ethernetAddress = nullptr;
        for (ARPEntry *current:*arpTable) {
            if (current->matches(receiverAddress)) {
                *ethernetAddress = current->getEthernetAddress();
                tableAccessLock->release();
                return true;
            }
        }
        tableAccessLock->release();
        return false;
    }

    ARPModule::ARPModule(NetworkEventBus *eventBus, EthernetDevice *outDevice) {
        this->eventBus = eventBus;
        this->outDevice = outDevice;
        arpTable = new Util::ArrayList<ARPEntry *>();
        timeService = System::getService<TimeService>();

        tableAccessLock = new Spinlock();
        tableAccessLock->release();
    }

    ARPModule::~ARPModule() {
        if (arpTable != nullptr) {
            ARPEntry *toDelete;
            for (size_t i = 0; i < arpTable->size(); i++) {
                toDelete = arpTable->get(i);
                arpTable->remove(i);
                i--;
                delete toDelete;
            }
        }
        delete arpTable;
    }

    uint8_t ARPModule::resolveTo(EthernetAddress **ethernetAddress, IP4Address *targetProtocolAddress,
                                 IP4Address *senderProtocolAddress) {
        if (ethernetAddress == nullptr || targetProtocolAddress == nullptr || senderProtocolAddress == nullptr) {
            log.error("At least one parameter was null, not resolving");
            return 1;
        }
        if (arpTable == nullptr || timeService == nullptr) {
            log.error("ARP table or time service was null, not resolving");
            return 1;
        }
        if (entryFound(ethernetAddress, targetProtocolAddress)) {
            return 0;
        }

        log.info("No entry found for %s, sending ARP request", (char *) targetProtocolAddress->asString());
        sendRequest(senderProtocolAddress, targetProtocolAddress);

        timeService->msleep(500);

        if (entryFound(ethernetAddress, targetProtocolAddress)) {
            return 0;
        }
        log.error("No ARP response arrived in time, no resolve for %s possible",
                  (char *) targetProtocolAddress->asString()
        );
        return 1;
    }

    uint8_t ARPModule::sendRequest(IP4Address *senderProtocolAddress, IP4Address *targetProtocolAddress) {
        if (senderProtocolAddress == nullptr || targetProtocolAddress == nullptr) {
            log.error("senderProtocolAddress or targetProtocolAddress was null, returning");
            return 1;
        }

        auto *broadcastAddress = EthernetAddress::buildBroadcastAddress();

        //See RFC 826 page 3 for details
        auto *arpRequest = new ARPMessage(
                1, // 1 for Ethernet
                (uint16_t) EthernetDataPart::EtherType::IP4, // 0x0800 for IPv4
                MAC_SIZE,
                IP4ADDRESS_LENGTH,
                ARPMessage::OpCode::REQUEST
        );

        uint8_t hardwareAddress[MAC_SIZE];
        uint8_t protocolAddress[IP4ADDRESS_LENGTH];

        outDevice->getAddress()->copyTo(hardwareAddress);
        arpRequest->setSenderHardwareAddress(hardwareAddress);

        senderProtocolAddress->copyTo(protocolAddress);
        arpRequest->setSenderProtocolAddress(protocolAddress);

        broadcastAddress->copyTo(hardwareAddress);
        arpRequest->setTargetHardwareAddress(hardwareAddress);

        targetProtocolAddress->copyTo(protocolAddress);
        arpRequest->setTargetProtocolAddress(protocolAddress);

        //Broadcast address will be deleted after sending here, all other addresses are just copied to ARP request
        //-> no other addresses will be deleted here!
        eventBus->publish(
                new EthernetSendEvent(outDevice, new EthernetFrame(broadcastAddress, arpRequest))
        );
        return 0;
    }

    uint8_t ARPModule::processIncoming(ARPMessage *message) {
        if (message == nullptr) {
            log.error("Incoming ARP message was null, ignoring");
            return 1;
        }
        uint8_t processErrors;
        switch (message->getOpCode()) {
            case ARPMessage::OpCode::REQUEST: {
                //Use incoming requests as updates
                processErrors = addEntry(new IP4Address(message->getSenderProtocolAddress()),
                                         new EthernetAddress(message->getSenderHardwareAddress())
                );
                if (processErrors) {
                    log.error("Could not process ARP Request: ARP table update failed!");
                    //Message will be deleted in IP4Module after processing
                    //-> no 'delete message' here!
                    return processErrors;
                }

                uint8_t myAddressAsBytes[MAC_SIZE];
                outDevice->getAddress()->copyTo(myAddressAsBytes);

                auto *response = message->buildReply(myAddressAsBytes);
                auto *outFrame =
                        new EthernetFrame(new EthernetAddress(myAddressAsBytes), response);
                eventBus->publish(
                        new EthernetSendEvent(outDevice, outFrame)
                );
                //Message will be deleted in IP4Module after processing
                //-> no 'delete message' here!
                break;
            }
            case ARPMessage::OpCode::REPLY: {
                processErrors = addEntry(new IP4Address(message->getSenderProtocolAddress()),
                                         new EthernetAddress(message->getSenderHardwareAddress())
                );
                if (processErrors) {
                    log.error(("Could not process ARP Response: ARP table update failed!"));
                    return processErrors;
                }
                //Message will be deleted in IP4Module after processing
                //-> no 'delete message' here!
                break;
            }
            default: {
                log.error("Invalid opCode in incoming ARP message, ignoring");
                //Message will be deleted in IP4Module after processing
                //-> no 'delete message' here!
                return 1;
            }
        }
        //Message will be deleted in IP4Module after processing
        //-> no 'delete message' here!
        return 0;
    }
}