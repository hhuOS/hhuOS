//
// Created by hannes on 20.04.21.
//

#ifndef HHUOS_IP_H
#define HHUOS_IP_H


#include "Command.h"
#include <kernel/service/NetworkService.h>
#include <device/network/loopback/Loopback.h>

class Ip : public Command {

/**
 * Implementation of Command.
 *
 * This application provides printing and editing attributes of available network interfaces
 */

public:

    /**
     * Default-constructor.
     */
    Ip() = delete;

    /**
     * Copy-constructor.
     */
    Ip(const Ip &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Ip(Shell &shell);

    /**
     * Print available network interfaces and their attributes
     *
     * @param networkService The network service you want to read interfaces from
     */
    void link(Kernel::NetworkService *networkService);

    /**
     * Format a given MAC address encoded in uint8_t as a String
     *
     * @param mac The MAC address you want to format.
     * @return The given MAC address as a String like "xx:xx:xx:xx:xx:xx".
     */
    static String getMACAsString(uint8_t *mac);

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;
};


#endif //HHUOS_IP_H
