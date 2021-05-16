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

void Kernel::EthernetModule::onEvent(const Kernel::Event &event) {
    if (event.getType() == EthernetSendEvent::TYPE) {
        auto sendEvent = ((EthernetSendEvent &) event);
        NetworkDevice *outInterface = sendEvent.getOutInterface();
        EthernetFrame *outFrame = sendEvent.getEthernetFrame();

        outInterface->sendPacket(outFrame->getDataAsByteBlock(), outFrame->getLength());
        return;
    }
    if(event.getType() == EthernetReceiveEvent::TYPE){
        auto receiveEvent = ((EthernetReceiveEvent &) event);
        EthernetFrame *inFrame = receiveEvent.getEthernetFrame();
        auto *eventBus = Kernel::System::getService<Kernel::EventBus>();

        switch(inFrame->getEtherType()){
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
