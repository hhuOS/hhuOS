//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETSENDEVENT_H
#define HHUOS_ETHERNETSENDEVENT_H


#include <device/network/NetworkDevice.h>
#include <kernel/network/internet/IP4Address.h>
#include <kernel/network/ethernet/EthernetFrame.h>
#include <kernel/event/Event.h>

namespace Kernel {

class EthernetSendEvent : public Event{
private:
    NetworkDevice *outInterface;
    EthernetFrame *ethernetFrame;

public:
    EthernetSendEvent(NetworkDevice *outInterface, EthernetFrame *ethernetFrame);

    String getType() const override;

    static const constexpr char *TYPE = "EthernetSendEvent";
};

}

#endif //HHUOS_ETHERNETSENDEVENT_H
