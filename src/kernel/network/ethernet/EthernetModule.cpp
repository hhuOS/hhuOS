//
// Created by hannes on 15.05.21.
//

#include <kernel/core/System.h>
#include <kernel/service/EventBus.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include <kernel/network/internet/IP4Datagram.h>

#include "EthernetModule.h"
#include "EthernetDevice.h"


Kernel::EthernetModule::EthernetModule(Kernel::EventBus *eventBus) {
    this->eventBus=eventBus;
    this->ethernetDevices = new Util::ArrayList<EthernetDevice *>();
}

Util::ArrayList<EthernetDevice *> *Kernel::EthernetModule::getEthernetDevices() const {
    return ethernetDevices;
}

void Kernel::EthernetModule::registerNetworkDevice(NetworkDevice *networkDevice) {
    for(uint32_t i=0;i<ethernetDevices->size();i++){
        //Return if an ethernet device connected to the same network device could be found
        if(ethernetDevices->get(i)->connectedTo(networkDevice)){
            return;
        }
    }
    //Add a new connected ethernet device if no duplicate found
    this->ethernetDevices->add(
            new EthernetDevice(networkDevice)
            );
}

void Kernel::EthernetModule::unregisterNetworkDevice(NetworkDevice *networkDevice) {
    for(uint32_t i=0;i<ethernetDevices->size();i++){
        if(ethernetDevices->get(i)->connectedTo(networkDevice)){
            ethernetDevices->remove(i);
            return;
        }
    }
}

void Kernel::EthernetModule::onEvent(const Kernel::Event &event) {
    if (event.getType() == EthernetSendEvent::TYPE) {
        log.info("EthernetSendEvent with EthernetFrame received");

        auto sendEvent = ((EthernetSendEvent &) event);
        EthernetDevice *outDevice = sendEvent.getOutDevice();
        EthernetFrame *outFrame = sendEvent.getEthernetFrame();

        if (outFrame != nullptr && outDevice != nullptr) {
            outDevice->sendEthernetFrame(outFrame);
        }

        return;
    }
    if (event.getType() == EthernetReceiveEvent::TYPE) {
        auto receiveEvent = ((EthernetReceiveEvent &) event);
        EthernetFrame *inFrame = receiveEvent.getEthernetFrame();

        switch (inFrame->getEtherType()) {
            case EtherType::IP4:
                eventBus->publish(
                        Util::SmartPointer<Kernel::Event>(
                                new Kernel::IP4ReceiveEvent(
                                        new IP4Datagram(inFrame->getDataPart())
                                )
                        )
                );
                return;
            case EtherType::ARP:
                eventBus->publish(
                        Util::SmartPointer<Kernel::Event>(
                                new Kernel::ARPReceiveEvent(
                                        new ARPResponse(inFrame->getDataPart())
                                )
                        )
                );
                return;
            case EtherType::IP6:
                break;
        }
    }
}
