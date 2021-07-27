//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4SENDEVENT_H
#define HHUOS_IP4SENDEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/internet/IP4Datagram.h>

namespace Kernel {
    class IP4SendEvent : public Event {
    private:
        IP4Address *destinationAddress;
        IP4DataPart *dataPart;

    public:
        explicit IP4SendEvent(IP4Address *destinationAddress, IP4DataPart *dataPart);

        [[nodiscard]] IP4Address *getDestinationAddress() const;

        [[nodiscard]] IP4DataPart *getDataPart() const;

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "IP4SendEvent";
    };
}

#endif //HHUOS_IP4SENDEVENT_H
