//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/UDP4ReceiveEvent.h>
#include "UDP4Module.h"

namespace Kernel {

    UDP4Module::UDP4Module(Kernel::NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
    }

    void UDP4Module::onEvent(const Kernel::Event &event) {
        if (event.getType() == UDP4ReceiveEvent::TYPE) {
            //auto *udpDatagram = ((UDP4ReceiveEvent &) event).getDatagram();
            return;
        }
    }

}