//
// Created by hannes on 15.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/service/TimeService.h>
#include "ARPModule.h"
#include "ARPMessage.h"

namespace Kernel {
    //Private method!
    uint8_t ARPModule::found(EthernetAddress **ethernetAddress, IP4Address *receiverAddress) {
        if(arpTable== nullptr || tableAccessLock== nullptr){
            return 1;
        }
        tableAccessLock->acquire();
        *ethernetAddress = nullptr;
        for (ARPEntry *current:*arpTable) {
            if (current->matches(receiverAddress)) {
                *ethernetAddress = current->getEthernetAddress();
                tableAccessLock->release();
                return 0;
            }
        }
        tableAccessLock->release();
        return 1;
    }

    ARPModule::ARPModule(NetworkEventBus *eventBus, EthernetDevice *outDevice) {
        this->eventBus = eventBus;
        this->outDevice = outDevice;
        arpTable = new Util::ArrayList<ARPEntry *>();
//        timeService=Kernel::System::getService<TimeService>();

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
        if (ethernetAddress== nullptr || targetProtocolAddress == nullptr || senderProtocolAddress == nullptr) {
            log.error("At least one parameter was null, not resolving");
            return 1;
        }
        if (arpTable == nullptr || timeService == nullptr) {
            log.error("ARP table or time service was null, not resolving");
            return 1;
        }
        if(found(ethernetAddress, targetProtocolAddress)) {
            return 0;
        }

        log.info("No entry found for %s, sending ARP request", (char *)targetProtocolAddress->asString());
        sendRequest(senderProtocolAddress, targetProtocolAddress);

        timeService->msleep(500);

        if(!found(ethernetAddress, targetProtocolAddress)){
            log.error("No ARP response arrived in time, no resolve for %s possible",
                      (char*)targetProtocolAddress->asString()
                      );
            return 1;
        }

        return 0;
    }

    void ARPModule::addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) {
        if (ip4Address == nullptr) {
            log.error("Given IP4 address was null, not adding entry");
            return;
        }
        if (ethernetAddress == nullptr) {
            log.error("Given Ethernet address was null, not adding entry");
        }
        if (arpTable == nullptr) {
            log.error("ARP table was null, not adding entry");
            return;
        }
        tableAccessLock->acquire();
        arpTable->add(new ARPEntry(ip4Address, ethernetAddress));
        tableAccessLock->release();
    }

    uint8_t ARPModule::sendRequest(IP4Address *senderProtocolAddress, IP4Address *targetProtocolAddress) {
        if(senderProtocolAddress == nullptr || targetProtocolAddress == nullptr){
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
                new EthernetSendEvent(outDevice,new EthernetFrame(broadcastAddress,arpRequest))
        );
        return 0;
    }
}