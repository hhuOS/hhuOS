//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ARPRECEIVEEVENT_H
#define HHUOS_ARPRECEIVEEVENT_H


#include <kernel/event/Event.h>
#include <kernel/network/internet/arp/ARPMessage.h>

namespace Kernel {

    class ARPReceiveEvent : public Event {
    private:
        ARPMessage *arpResponse;

    public:
        explicit ARPReceiveEvent(ARPMessage *arpMessage);

        [[nodiscard]] ARPMessage *getARPMessage() const;

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "ARPReceiveEvent";
    };

}

#endif //HHUOS_ARPRECEIVEEVENT_H
