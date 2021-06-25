//
// Created by hannes on 01.06.21.
//

#include "NetworkEventBus.h"

namespace Kernel {
    NetworkEventBus::NetworkEventBus(EventBus *eventBus) : eventBus(eventBus) {}

    void NetworkEventBus::publish(Event *event) {
        this->eventBus->publish(
                Util::SmartPointer<Event>(event)
        );
    }

    void NetworkEventBus::subscribe(Receiver &receiver, const String &type) {
        this->eventBus->subscribe(receiver, type);
    }

    void NetworkEventBus::unsubscribe(Receiver &receiver, const String &type) {
        this->eventBus->unsubscribe(receiver, type);
    }
}