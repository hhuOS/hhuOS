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


Kernel::EthernetModule::EthernetModule() {
    this->ethernetDevices = new Util::ArrayList<EthernetDevice *>();
}

Util::ArrayList<EthernetDevice *> *Kernel::EthernetModule::getEthernetDevices() const {
    return ethernetDevices;
}

void Kernel::EthernetModule::registerEthernetDevice(EthernetDevice *ethernetDevice) {
    this->ethernetDevices->add(ethernetDevice);
}

void Kernel::EthernetModule::unregisterEthernetDevice(EthernetDevice *ethernetDevice) {
    this->ethernetDevices->remove(ethernetDevice);
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
        auto *eventBus = Kernel::System::getService<Kernel::EventBus>();

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
