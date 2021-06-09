//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4RECEIVEEVENT_H
#define HHUOS_ICMP4RECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/internet/IP4DataPart.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/internet/icmp/ICMP4Message.h>
#include <kernel/network/internet/IP4Datagram.h>

namespace Kernel {

    class ICMP4ReceiveEvent : public Event {
    private:
        IP4Datagram *datagram = nullptr;
        NetworkByteBlock *input = nullptr;

    public:

        explicit ICMP4ReceiveEvent(IP4Datagram *datagram, NetworkByteBlock *input);

        [[nodiscard]] String getType() const override;

        [[nodiscard]] NetworkByteBlock *getInput() const;

        [[nodiscard]] IP4Datagram *getDatagram() const;

        static const constexpr char *TYPE = "ICMP4ReceiveEvent";
    };

}


#endif //HHUOS_ICMP4RECEIVEEVENT_H
