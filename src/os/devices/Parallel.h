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
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class Parallel {

public:

    /**
     * Starting addresses of the registers of each port.
     */
    enum LptPort {
        LPT1 = 1,
        LPT2 = 2,
        LPT3 = 3
    };

    /**
     * Operating modes of an LPT-port.
     */
    enum ParallelMode {
        SPP = 0,
        EPP = 1
    };

    /**
     * Check if an LPT-port exists.
     * Always check if the LPT-port exists before creating an instance of this class!
     *
     * @param port The port
     */
    static bool checkPort(LptPort port);

    /**
     * Constructor.
     *
     * @param port The port
     * @param mode The mode, in which the port should operate
     */
    explicit Parallel(LptPort port, ParallelMode mode = SPP);

    /**
     * Destructor.
     */
    ~Parallel() = default;

    /**
     * Change the parallel port's operating mode
     *
     * @param mode The mode
     */
    void setMode(ParallelMode mode);

    /**
     * Get the port's operating mode.
     */
    ParallelMode getMode();

    /**
     * Send an init-signal to the printer via the control-port.
     * This tells the printer, that a new communication is about to start.
     */
    void initializePort();

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
     * Send a character to the remote device.
     *
     * @param c The character
     */
    void sendChar(char c);

    /**
     * Read a character from the remote device.
     *
     * @param c
     */
    char readChar();

    /**
     * Send a data buffer to the remote device.
     *
     * @param data The buffer
     * @param len The amount of bytes to be sent
     */
    void sendData(char *data, uint32_t len);

    /**
     * Read data from the remote device.
     *
     * @param data The buffer
     * @param len The amount of bytes to be read
     */
    void readData(char *data, uint32_t len);

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
    ParallelMode mode;

    IOport sppDataPort;
    IOport sppStatusPort;
    IOport sppControlPort;
    IOport eppAddressPort;
    IOport eppDataPort;
};

#endif
