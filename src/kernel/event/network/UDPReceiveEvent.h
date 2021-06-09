//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDPRECEIVEEVENT_H
#define HHUOS_UDPRECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/udp/UDPDatagram.h>
#include <kernel/network/internet/IP4Datagram.h>

namespace Kernel {

    class UDPReceiveEvent : public Event {
    private:
        UDPDatagram *udpDatagram;
        IP4Datagram *ip4Datagram;
        NetworkByteBlock *input;

    public:
        explicit UDPReceiveEvent(UDPDatagram *udpDatagram, IP4Datagram *ip4Datagram, NetworkByteBlock *input);

        UDPDatagram *getUDPDatagram();

        [[nodiscard]] NetworkByteBlock *getInput() const;

        [[nodiscard]] String getType() const override;

        [[nodiscard]] IP4Datagram *getIp4Datagram() const;

        static const constexpr char *TYPE = "UDPReceiveEvent";
    };

}

#endif //HHUOS_UDPRECEIVEEVENT_H
