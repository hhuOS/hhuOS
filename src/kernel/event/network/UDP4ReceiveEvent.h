//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDP4RECEIVEEVENT_H
#define HHUOS_UDP4RECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/udp/UDP4Header.h>
#include <kernel/network/internet/IP4Header.h>

namespace Kernel {
    class UDP4ReceiveEvent : public Event {
    private:
        UDP4Header *udp4Header;
        IP4Header *ip4Header;
        NetworkByteBlock *input;

    public:
        explicit UDP4ReceiveEvent(IP4Header *ip4Header, UDP4Header *udp4header, NetworkByteBlock *input);

        [[nodiscard]] IP4Header *getIP4Header() const;

        [[nodiscard]] UDP4Header *getUDP4Header();

        [[nodiscard]] NetworkByteBlock *getInput() const;

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "UDP4ReceiveEvent";
    };
}

#endif //HHUOS_UDP4RECEIVEEVENT_H
