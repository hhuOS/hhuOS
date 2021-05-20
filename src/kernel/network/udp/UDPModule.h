//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDPMODULE_H
#define HHUOS_UDPMODULE_H

#include <kernel/log/Logger.h>
#include <kernel/event/Event.h>
#include <kernel/event/Receiver.h>

namespace Kernel {

    class UDPModule : public Receiver {
    public:
        Logger &log = Logger::get("UDPModule");

        /**
         * Inherited method from Receiver.
         * This method is meant to be overridden and
         * implemented by this class.
         */
        void onEvent(const Event &event) override;
    };

}

#endif //HHUOS_UDPMODULE_H
