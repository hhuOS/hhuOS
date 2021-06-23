//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_IP4RECEIVEEVENT_H
#define HHUOS_IP4RECEIVEEVENT_H

#include <kernel/event/Event.h>
#include <kernel/network/internet/IP4Header.h>

namespace Kernel {
    class IP4ReceiveEvent : public Event {
    private:
        NetworkByteBlock *input;

    public:
        explicit IP4ReceiveEvent(NetworkByteBlock *input);

        [[nodiscard]] NetworkByteBlock *getInput() const;

        [[nodiscard]] String getType() const override;

        static const constexpr char *TYPE = "IP4ReceiveEvent";
    };
}

#endif //HHUOS_IP4RECEIVEEVENT_H
