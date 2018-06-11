/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_PARALLEL_H
#define HHUOS_PARALLEL_H

#include <kernel/IOport.h>
#include <kernel/services/TimeService.h>

/**
 * Driver for the parallel LPT-ports.
 */
class Parallel {

public:

    /**
     * Starting addresses of the registers of each port.
     */
    enum LptPort {
        LPT1 = 0,
        LPT2 = 1,
        LPT3 = 2
    };

    static bool checkPort(LptPort port);

    /**
     * Constructor.
     *
     * @param port The port
     */
    explicit Parallel(LptPort port);

    /**
     * Destructor.
     */
    ~Parallel() = default;

    /**
     * Send an init-signal to the printer via the control-port.
     * This tells the printer, that a new communication is about to start.
     */
    void initializePrinter();

    /**
     * Check, if the printer is currently busy.
     */
    bool isBusy();

    /**
     * Check, if the printer has paper.
     */
    bool isPaperEmpty();

    /**
     * Check, if the printer is in an error state.
     */
    bool checkError();

    /**
     * Send a character to the printer.
     *
     * @param c The character
     */
    void sendChar(char c);

    /**
     * Send a data buffer to the printer.
     *
     * @param data The buffer
     * @param len The amount of bytes to be sent
     */
    void sendData(char *data, uint32_t len);

    /**
     * Get the port number.
     */
    LptPort getPortNumber();

private:

    /**
     * Calculate the base address of the IO-ports of a given parallel port.
     *
     * @param port The port
     * @return The base address
     */
    static uint16_t getBasePort(LptPort port);

private:

    TimeService *timeService = nullptr;

    LptPort port;

    IOport dataPort;
    IOport statusPort;
    IOport controlPort;
};

#endif
