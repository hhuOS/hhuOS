//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_IP4RECEIVEEVENT_H
#define HHUOS_IP4RECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/internet/IP4Header.h>

namespace Kernel {
    class IP4ReceiveEvent : public Event {
    private:
        IP4Header *ip4Header;
        NetworkByteBlock *input;

    public:
        explicit IP4ReceiveEvent(IP4Header *ip4Header, NetworkByteBlock *input);

        IP4Header *getHeader();

        [[nodiscard]] NetworkByteBlock *getInput() const;

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "IP4ReceiveEvent";
    };
}

#endif //HHUOS_IP4RECEIVEEVENT_H
