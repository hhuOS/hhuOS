//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4MODULE_H
#define HHUOS_ICMP4MODULE_H

#include <kernel/service/EventBus.h>
#include <kernel/log/Logger.h>
#include "ICMP4MessageType.h"

namespace Kernel {

    class ICMP4Module : public Receiver{
    private:
        EventBus *eventBus;
    public:
        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("ICMP4Module");


/**
     * Inherited method from Receiver.
     * This method is meant to be overridden and
     * implemented by this class.
     */
        void onEvent(const Event &event) override;
    };
}

#endif //HHUOS_ICMP4MODULE_H
