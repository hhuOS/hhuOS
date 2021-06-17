//
// Created by hannes on 01.06.21.
//

#ifndef HHUOS_NETWORKEVENTBUS_H
#define HHUOS_NETWORKEVENTBUS_H


#include <kernel/service/EventBus.h>

namespace Kernel {
    class NetworkEventBus {
    private:
        EventBus *eventBus;

    public:
        explicit NetworkEventBus(EventBus *eventBus);

        void publish(Event *event);

        /**
         * Subscribes to a certain type of Event.
         *
         * @param receiver The Receiver
         * @param type The Event type
         */
        void subscribe(Receiver &receiver, const String &type);

        /**
         * Unsubscribes from a certain type of Event.
         *
         * @param receiver The Receiver
         * @param type The Event type
         */
        void unsubscribe(Receiver &receiver, const String &type);
    };
}

#endif //HHUOS_NETWORKEVENTBUS_H
