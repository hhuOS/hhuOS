//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/UDPReceiveEvent.h>
#include "UDPModule.h"

namespace Kernel {

    UDPModule::UDPModule(Kernel::NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
    }

    void UDPModule::onEvent(const Kernel::Event &event) {
        if (event.getType() == UDPReceiveEvent::TYPE) {
            //auto *udpDatagram = ((UDPReceiveEvent &) event).getDatagram();
            return;
        }
    }

}