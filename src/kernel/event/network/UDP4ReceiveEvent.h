//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDPRECEIVEEVENT_H
#define HHUOS_UDPRECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/udp/UDP4Datagram.h>
#include <kernel/network/internet/IP4Datagram.h>

namespace Kernel {

    class UDP4ReceiveEvent : public Event {
    private:
        UDP4Datagram *udpDatagram;
        IP4Datagram *ip4Datagram;
        NetworkByteBlock *input;

    public:
        explicit UDP4ReceiveEvent(UDP4Datagram *udpDatagram, IP4Datagram *ip4Datagram, NetworkByteBlock *input);

        UDP4Datagram *getUDP4Datagram();

        [[nodiscard]] NetworkByteBlock *getInput() const;

        [[nodiscard]] String getType() const override;

        [[nodiscard]] IP4Datagram *getIP4Datagram() const;

        static const constexpr char *TYPE = "UDP4ReceiveEvent";
    };

}

#endif //HHUOS_UDPRECEIVEEVENT_H
