//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ARPRECEIVEEVENT_H
#define HHUOS_ARPRECEIVEEVENT_H


#include <kernel/event/Event.h>
#include <kernel/network/internet/arp/ARPResponse.h>

namespace Kernel {

    class ARPReceiveEvent : public Event{
    private:
        ARPResponse *arpResponse;

    public:
        ARPReceiveEvent(ARPResponse *arpResponse);

        ARPResponse *getArpResponse() const;

        String getType() const override;

        static const constexpr char *TYPE = "ARPReceiveEvent";
    };

}

#endif //HHUOS_ARPRECEIVEEVENT_H
