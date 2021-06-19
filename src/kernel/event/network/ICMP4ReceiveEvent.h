//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4RECEIVEEVENT_H
#define HHUOS_ICMP4RECEIVEEVENT_H

#include <kernel/network/internet/IP4Header.h>
#include <kernel/event/Event.h>

namespace Kernel {
    class ICMP4ReceiveEvent : public Event {
    private:
        IP4Header *ip4Header = nullptr;
        NetworkByteBlock *input = nullptr;

    public:

        explicit ICMP4ReceiveEvent(IP4Header *ip4Header, NetworkByteBlock *input);

        [[nodiscard]] String getType() const override;

        [[nodiscard]] NetworkByteBlock *getInput() const;

        [[nodiscard]] IP4Header *getIP4Header() const;

        static const constexpr char *TYPE = "ICMP4ReceiveEvent";
    };
}


#endif //HHUOS_ICMP4RECEIVEEVENT_H
