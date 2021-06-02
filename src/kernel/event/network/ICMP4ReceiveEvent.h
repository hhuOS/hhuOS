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
        GenericICMP4Message *genericIcmp4Message = nullptr;

    public:

        ICMP4ReceiveEvent(GenericICMP4Message *genericIcmp4Message);

        GenericICMP4Message *getGenericIcmp4Message() const;

        String getType() const override;

        static const constexpr char *TYPE = "ICMP4ReceiveEvent";
    };

}


#endif //HHUOS_ICMP4RECEIVEEVENT_H
