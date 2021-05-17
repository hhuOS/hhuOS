//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4RECEIVEEVENT_H
#define HHUOS_ICMP4RECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/internet/IP4DataPart.h>

namespace Kernel {

    class ICMP4ReceiveEvent : public Event {
    private:
        //We do not know the exact type of ICMP message here
        //-> Parsing needs to be done in ICMP Module later
        IP4DataPart *ip4DataPart;
    public:

        ICMP4ReceiveEvent(IP4DataPart *ip4DataPart);

        IP4DataPart *getIp4DataPart() const;

        String getType() const override;

        static const constexpr char *TYPE = "ICMP4ReceiveEvent";
    };

}


#endif //HHUOS_ICMP4RECEIVEEVENT_H
