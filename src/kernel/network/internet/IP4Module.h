//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4MODULE_H
#define HHUOS_IP4MODULE_H


#include <kernel/event/Receiver.h>
#include <kernel/log/Logger.h>

namespace Kernel {

class IP4Module final : public Receiver {
    /**
     * A logger to provide information on the kernel log.
     */
    Logger &log = Logger::get("IP4Module");

    /**
     * Inherited method from Receiver.
     * This method is meant to be overridden and
     * implemented by this class.
     */
    void onEvent(const Event &event) override;
};

}


#endif //HHUOS_IP4MODULE_H
