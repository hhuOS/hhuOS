//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4RECEIVEEVENT_H
#define HHUOS_ICMP4RECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/internet/IP4DataPart.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/internet/icmp/ICMP4Message.h>

namespace Kernel {

    class ICMP4ReceiveEvent : public Event {
    private:
        ICMP4Message *icmp4Message = nullptr;

    public:

        explicit ICMP4ReceiveEvent(ICMP4Message *icmp4Message);

        [[nodiscard]] ICMP4Message * getIcmp4Message() const;

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "ICMP4ReceiveEvent";
    };

}


#endif //HHUOS_ICMP4RECEIVEEVENT_H
