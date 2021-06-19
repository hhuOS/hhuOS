//
// Created by hannes on 06.06.21.
//

#ifndef HHUOS_ICMP4SENDEVENT_H
#define HHUOS_ICMP4SENDEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/internet/icmp/ICMP4Message.h>

namespace Kernel {
    class ICMP4SendEvent : public Event {
    private:
        IP4Address *destinationAddress = nullptr;
        ICMP4Message *icmp4Message = nullptr;
    public:
        explicit ICMP4SendEvent(IP4Address *destinationAddress, ICMP4Message *icmp4Message);

        [[nodiscard]] ICMP4Message *getIcmp4Message() const;

        [[nodiscard]] IP4Address *getDestinationAddress() const;

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "ICMP4SendEvent";
    };
}

#endif //HHUOS_ICMP4SENDEVENT_H
