//
// Created by hannes on 01.06.21.
//

#include "NetworkEventBus.h"

namespace Kernel {
    NetworkEventBus::NetworkEventBus(Kernel::EventBus *eventBus) : eventBus(eventBus) {}

    void NetworkEventBus::publish(Kernel::Event *event) {
        this->eventBus->publish(
                Util::SmartPointer<Kernel::Event>(event)
        );
    }

    void NetworkEventBus::subscribe(Kernel::Receiver &receiver, const String &type) {
        this->eventBus->subscribe(receiver, type);
    }

    void NetworkEventBus::unsubscribe(Kernel::Receiver &receiver, const String &type) {
        this->eventBus->unsubscribe(receiver, type);
    }
}