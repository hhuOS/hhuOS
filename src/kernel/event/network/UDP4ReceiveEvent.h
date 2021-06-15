//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDP4RECEIVEEVENT_H
#define HHUOS_UDP4RECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/udp/UDP4Datagram.h>
#include <kernel/network/internet/IP4Datagram.h>

namespace Kernel {

    class UDP4ReceiveEvent : public Event {
    private:
        UDP4Header *udp4Header;
        IP4Header *ip4Header;
        NetworkByteBlock *input;

    public:
        explicit UDP4ReceiveEvent(UDP4Header *udp4header, IP4Header *ip4Header, NetworkByteBlock *input);

        UDP4Header * getUDP4Datagram();

        [[nodiscard]] NetworkByteBlock *getInput() const;

        [[nodiscard]] String getType() const override;

        [[nodiscard]] IP4Header * getIP4Header() const;

        static const constexpr char *TYPE = "UDP4ReceiveEvent";
    };

}

#endif //HHUOS_UDP4RECEIVEEVENT_H
