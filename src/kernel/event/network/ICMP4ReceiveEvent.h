//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4RECEIVEEVENT_H
#define HHUOS_ICMP4RECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/internet/IP4DataPart.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/internet/icmp/messages/GenericICMP4Message.h>

namespace Kernel {

    class ICMP4ReceiveEvent : public Event {
    private:
        IP4Address *sourceAddress;
        IP4Address *destinationAddress;

        //We do not know the exact type of ICMP message here
        //-> Parsing needs to be done in ICMP Module later
        IP4DataPart *ip4DataPart;

        GenericICMP4Message *genericIcmp4Message = nullptr;
    public:

        ICMP4ReceiveEvent(IP4Address *sourceAddress, IP4Address *destinationAddress, IP4DataPart *ip4DataPart);

        ICMP4ReceiveEvent(GenericICMP4Message *genericIcmp4Message);

        IP4Address *getSourceAddress() const;

        IP4Address *getDestinationAddress() const;

        IP4DataPart *getIp4DataPart() const;

        GenericICMP4Message *getGenericIcmp4Message() const;

        String getType() const override;

        static const constexpr char *TYPE = "ICMP4ReceiveEvent";
    };

}


#endif //HHUOS_ICMP4RECEIVEEVENT_H
