//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ETHERNETRECEIVEEVENT_H
#define HHUOS_ETHERNETRECEIVEEVENT_H


#include <kernel/event/Event.h>
#include <kernel/network/ethernet/EthernetHeader.h>

namespace Kernel {
    class EthernetReceiveEvent : public Event {
    private:
        NetworkByteBlock *input;

    public:
        explicit EthernetReceiveEvent(NetworkByteBlock *input);

        [[nodiscard]] String getType() const override;

        [[nodiscard]] NetworkByteBlock *getInput() const;

        static const constexpr char *TYPE = "EthernetReceiveEvent";
    };

}

#endif //HHUOS_ETHERNETRECEIVEEVENT_H
