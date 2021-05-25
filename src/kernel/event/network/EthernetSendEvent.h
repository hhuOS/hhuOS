//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETSENDEVENT_H
#define HHUOS_ETHERNETSENDEVENT_H


#include <device/network/NetworkDevice.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/ethernet/EthernetFrame.h>
#include <kernel/event/Event.h>
#include <kernel/network/ethernet/EthernetDevice.h>

namespace Kernel {

    class EthernetSendEvent : public Event {
    private:
        EthernetDevice *outDevice;
        EthernetFrame *ethernetFrame;

    public:
        EthernetSendEvent(EthernetDevice *outDevice, EthernetFrame *ethernetFrame);

        EthernetDevice *getOutDevice() const;

        EthernetFrame *getEthernetFrame() const;

        String getType() const override;

        static const constexpr char *TYPE = "EthernetSendEvent";
    };

}

#endif //HHUOS_ETHERNETSENDEVENT_H
