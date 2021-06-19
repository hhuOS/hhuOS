//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETSENDEVENT_H
#define HHUOS_ETHERNETSENDEVENT_H


#include <kernel/network/ethernet/EthernetDevice.h>
#include <kernel/event/Event.h>

namespace Kernel {
    class EthernetSendEvent : public Event {
    private:
        EthernetDevice *outDevice;
        EthernetFrame *ethernetFrame;

    public:
        EthernetSendEvent(EthernetDevice *outDevice, EthernetFrame *ethernetFrame);

        [[nodiscard]] EthernetDevice *getOutDevice() const;

        [[nodiscard]] EthernetFrame *getEthernetFrame() const;

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "EthernetSendEvent";
    };

}

#endif //HHUOS_ETHERNETSENDEVENT_H
