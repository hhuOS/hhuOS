//
// Created by hannes on 15.05.21.
//

#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/core/System.h>
#include <kernel/service/EventBus.h>
#include <kernel/network/internet/IP4Datagram.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/network/arp/ARPResponse.h>
#include <kernel/event/network/ARPReceiveEvent.h>
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

        if(inFrame->getProtocolType() == EthernetFrame::ETHERTYPE_IP4){
            auto *inDatagram = new IP4Datagram(inFrame->getDataPart());
            eventBus->publish(
                    Util::SmartPointer<Kernel::Event>(
                            new Kernel::IP4ReceiveEvent(inDatagram)
                    )
            );
            return;
        }
        if(inFrame->getProtocolType() == EthernetFrame::ETHERTYPE_ARP){
            auto *inARPResponse = new ARPResponse(inFrame->getDataPart());
            eventBus->publish(
                    Util::SmartPointer<Kernel::Event>(
                            new Kernel::ARPReceiveEvent(inARPResponse)
                    )
            );
            return;
        }
    }
}
