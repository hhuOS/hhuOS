//
// Created by hannes on 17.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include "ICMP4Module.h"

void Kernel::ICMP4Module::onEvent(const Kernel::Event &event) {
    if(event.getType()==ICMP4ReceiveEvent::TYPE){

    }
}
