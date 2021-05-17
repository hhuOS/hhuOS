//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4ECHOREPLYEVENT_H
#define HHUOS_ICMP4ECHOREPLYEVENT_H


#include <kernel/network/internet/icmp/ICMP4EchoReply.h>
#include <lib/string/String.h>
#include <kernel/event/Event.h>

namespace Kernel{
    class ICMP4EchoReplyEvent : public Event{
    private:
        ICMP4EchoReply *reply;
    public:

        ICMP4EchoReplyEvent(ICMP4EchoReply *reply);

        ICMP4EchoReply *getReply() const;

        String getType() const override;

        static const constexpr char *TYPE = "ICMP4EchoReplyEvent";
    };
};

#endif //HHUOS_ICMP4ECHOREPLYEVENT_H
