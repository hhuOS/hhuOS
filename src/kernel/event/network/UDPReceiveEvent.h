//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDPRECEIVEEVENT_H
#define HHUOS_UDPRECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/udp/UDPDatagram.h>

namespace Kernel {

    class UDPReceiveEvent : public Event {
    private:
        UDPDatagram *datagram;
        NetworkByteBlock *input;

    public:
        explicit UDPReceiveEvent(UDPDatagram *datagram, NetworkByteBlock *input);

        UDPDatagram *getDatagram();

        NetworkByteBlock *getInput() const;

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "UDPReceiveEvent";
    };

}

#endif //HHUOS_UDPRECEIVEEVENT_H
