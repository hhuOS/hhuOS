//
// Created by hannes on 10.06.21.
//

#ifndef HHUOS_UDP4SENDEVENT_H
#define HHUOS_UDP4SENDEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/internet/IP4Address.h>
#include <kernel/network/udp/UDP4Datagram.h>

namespace Kernel {
    class UDP4SendEvent : public Event {
    private:
        IP4Address *destinationAddress = nullptr;
        uint16_t sourcePort = 0, destinationPort = 0;
        NetworkByteBlock *outData = nullptr;

    public:
        explicit UDP4SendEvent(
                IP4Address *destinationAddress, uint16_t sourcePort, uint16_t destinationPort,
                NetworkByteBlock *outData);

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "UDP4SendEvent";

        IP4Address *getDestinationAddress();

        [[nodiscard]] NetworkByteBlock *getOutData() const;

        [[nodiscard]] uint16_t getSourcePort() const;

        [[nodiscard]] uint16_t getDestinationPort() const;
    };
}

#endif //HHUOS_UDP4SENDEVENT_H
