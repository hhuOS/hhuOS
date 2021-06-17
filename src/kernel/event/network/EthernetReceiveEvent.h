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
        EthernetHeader *ethernetHeader;
        NetworkByteBlock *input;

    public:
        explicit EthernetReceiveEvent(EthernetHeader *ethernetHeader, NetworkByteBlock *input);

        [[nodiscard]] EthernetHeader * getEthernetHeader() const;

        [[nodiscard]] String getType() const override;

        [[nodiscard]] NetworkByteBlock *getInput() const;

        static const constexpr char *TYPE = "EthernetReceiveEvent";
    };

}

#endif //HHUOS_ETHERNETRECEIVEEVENT_H
