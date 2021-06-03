//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ETHERNETRECEIVEEVENT_H
#define HHUOS_ETHERNETRECEIVEEVENT_H


#include <device/network/NetworkDevice.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/ethernet/EthernetFrame.h>
#include <kernel/event/Event.h>

namespace Kernel {

    class EthernetReceiveEvent : public Event {
    private:
        EthernetFrame *ethernetFrame;

    public:
        explicit EthernetReceiveEvent(EthernetFrame *ethernetFrame);

        [[nodiscard]] EthernetFrame *getEthernetFrame() const;

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "EthernetReceiveEvent";
    };

}

#endif //HHUOS_ETHERNETRECEIVEEVENT_H
