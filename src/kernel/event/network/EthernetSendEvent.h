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
        EthernetAddress *targetHardwareAddress;
        EthernetDataPart *dataPart;

    public:
        EthernetSendEvent(EthernetDevice *outDevice, EthernetAddress *targetHardwareAddress,
                          EthernetDataPart *dataPart);

        [[nodiscard]] EthernetDevice *getOutDevice() const;

        [[nodiscard]] EthernetAddress *getTargetHardwareAddress() const;

        [[nodiscard]] EthernetDataPart *getDataPart() const;

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "EthernetSendEvent";
    };

}

#endif //HHUOS_ETHERNETSENDEVENT_H
