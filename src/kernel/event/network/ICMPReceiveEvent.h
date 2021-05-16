//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMPRECEIVEEVENT_H
#define HHUOS_ICMPRECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/internet/IP4DataPart.h>

namespace Kernel {

    class ICMPReceiveEvent : public Event {
    private:
        //We do not know the exact type of ICMP message here
        //-> Parsing needs to be done in ICMP Module later
        IP4DataPart *ip4DataPart;
    public:

        ICMPReceiveEvent(IP4DataPart *ip4DataPart);

        IP4DataPart *getIp4DataPart() const;

        String getType() const override;

        static const constexpr char *TYPE = "ICMPReceiveEvent";
    };

}


#endif //HHUOS_ICMPRECEIVEEVENT_H
