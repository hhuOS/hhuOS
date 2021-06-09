//
// Created by hannes on 10.06.21.
//

#ifndef HHUOS_UDP4SENDEVENT_H
#define HHUOS_UDP4SENDEVENT_H

#include <cstdint>
#include <kernel/event/Event.h>
#include <kernel/network/udp/UDP4Datagram.h>

namespace Kernel {

    class UDP4SendEvent : public Event {
    private:
        UDP4Datagram *datagram;

    public:
        explicit UDP4SendEvent(UDP4Datagram *datagram);

        UDP4Datagram *getDatagram();

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "UDP4SendEvent";
    };

}

#endif //HHUOS_UDP4SENDEVENT_H
