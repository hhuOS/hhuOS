//
// Created by hannes on 01.06.21.
//

#ifndef HHUOS_NETWORKEVENTBUS_H
#define HHUOS_NETWORKEVENTBUS_H


#include <kernel/service/EventBus.h>

namespace Kernel {
    class NetworkEventBus {
    private:
        Kernel::EventBus *eventBus;

    public:
        NetworkEventBus(Kernel::EventBus *eventBus);

        void publish(Kernel::Event *event);

        /**
         * Subscribes to a certain type of Event.
         *
         * @param receiver The Receiver
         * @param type The Event type
         */
        void subscribe(Kernel::Receiver &receiver, const String &type);

        /**
         * Unsubscribes from a certain type of Event.
         *
         * @param receiver The Receiver
         * @param type The Event type
         */
        void unsubscribe(Kernel::Receiver &receiver, const String &type);
    };
}

#endif //HHUOS_NETWORKEVENTBUS_H
