//
// Created by hannes on 15.05.21.
//

#include <kernel/event/network/EthernetSendEvent.h>
#include "EthernetModule.h"

void Kernel::EthernetModule::onEvent(const Kernel::Event &event) {
    if((event.getType()==EthernetSendEvent::TYPE)){
        auto sendEvent=((EthernetSendEvent &) event);
        NetworkDevice *outInterface=sendEvent.getOutInterface();
        EthernetFrame *outFrame=sendEvent.getEthernetFrame();

        outInterface->sendPacket(outFrame->getDataAsByteBlock(),outFrame->getLength());
    }
}
