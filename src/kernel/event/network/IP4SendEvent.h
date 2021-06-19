//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4SENDEVENT_H
#define HHUOS_IP4SENDEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/internet/IP4Datagram.h>

namespace Kernel {
    class IP4SendEvent : public Event {
    private:
        IP4Datagram *datagram;

    public:
        explicit IP4SendEvent(IP4Datagram *datagram);

        IP4Datagram *getDatagram();

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "IP4SendEvent";
    };
}

#endif //HHUOS_IP4SENDEVENT_H
