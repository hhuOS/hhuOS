//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_PING_H
#define HHUOS_PING_H


#include <kernel/network/NetworkEventBus.h>
#include <kernel/service/TimeService.h>
#include "Command.h"

class Ping : public Command {

/**
 * Implementation of Command.
 *
 * This application provides IP protocol stack testing via ICMP Echo and ICMP Echo Reply
 */

private:
    Kernel::NetworkEventBus *eventBus = nullptr;
    Kernel::TimeService *timeService = nullptr;

public:

    /**
     * Default-constructor.
     */
    Ping() = delete;

    /**
     * Copy-constructor.
     */
    Ping(const Ping &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Ping(Shell &shell);

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;
};


#endif //HHUOS_PING_H
