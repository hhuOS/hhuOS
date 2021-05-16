//
// Created by hannes on 15.05.21.
//

#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/core/System.h>
#include <kernel/service/EventBus.h>
#include "EthernetModule.h"

void Kernel::EthernetModule::onEvent(const Kernel::Event &event) {
    if (event.getType() == EthernetSendEvent::TYPE) {
        auto sendEvent = ((EthernetSendEvent &) event);
        NetworkDevice *outInterface = sendEvent.getOutInterface();
        EthernetFrame *outFrame = sendEvent.getEthernetFrame();

        outInterface->sendPacket(outFrame->getDataAsByteBlock(), outFrame->getLength());
    }
    if(event.getType() == EthernetReceiveEvent::TYPE){
        auto receiveEvent = ((EthernetReceiveEvent &) event);
        EthernetFrame *inframe = receiveEvent.getEthernetFrame();
        auto *eventBus = Kernel::System::getService<Kernel::EventBus>();

        if(inframe->getProtocolType() == EthernetFrame::ETHERTYPE_IP4){
            //Send contained IP4Datagram as an Event to IP4Module
        }
    }
}
