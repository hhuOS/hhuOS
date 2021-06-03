//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_IP4RECEIVEEVENT_H
#define HHUOS_IP4RECEIVEEVENT_H

#include <cstdint>
#include <kernel/event/Event.h>
#include <kernel/network/internet/IP4Datagram.h>

namespace Kernel {

    class IP4ReceiveEvent : public Event {
    private:
        IP4Datagram *datagram;

    public:
        explicit IP4ReceiveEvent(IP4Datagram *datagram);

        IP4Datagram *getDatagram();

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "IP4ReceiveEvent";
    };

}

#endif //HHUOS_IP4RECEIVEEVENT_H
