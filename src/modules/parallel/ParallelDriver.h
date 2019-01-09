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

#ifndef HHUOS_PARALLELDRIVER_H
#define HHUOS_PARALLELDRIVER_H

#include <kernel/Kernel.h>
#include <devices/cpu/IOport.h>
#include <kernel/services/TimeService.h>
#include <devices/ports/Port.h>
#include "Common.h"

namespace Parallel {

/**
 * Driver for the parallel LPT-ports.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
template<LptPort port>
class ParallelDriver : public Port {

public:

    /**
     * Check if an LPT-port exists.
     * Always check if the LPT-port exists before creating an instance of this class!
     */
    static bool checkPort();

    /**
     * Constructor.
     *
     * @param mode The mode, in which the port should operate
     */
    explicit ParallelDriver(ParallelMode mode = SPP);

    /**
     * Destructor.
     */
    ~ParallelDriver() = default;

    /**
     * Overriding function from IODevice.
     */
    void sendChar(char c) override;

    /**
     * Overriding function from IODevice.
     */
    char readChar() override;

    /**
     * Overriding function from IODevice.
     */
    void sendData(char *data, uint32_t len) override;

    /**
     * Overriding function from Port.
     */
    String getName() override;

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
     * Get the port number.
     */
    LptPort getPortNumber();

private:

    TimeService *timeService = nullptr;

    ParallelMode mode;

    IOport sppDataPort;
    IOport sppStatusPort;
    IOport sppControlPort;
    IOport eppAddressPort;
    IOport eppDataPort;

    static const constexpr char *NAME_1 = "Lpt1";
    static const constexpr char *NAME_2 = "Lpt2";
    static const constexpr char *NAME_3 = "Lpt3";
};

template<LptPort port>
bool ParallelDriver<port>::checkPort() {
    IOport controlPort(port + 2);

    // Toggle the automatic linefeed bit in the control register and check if it stays toggled.
    // If yes, there is a port.

    uint8_t tmp = controlPort.inb();

    tmp ^= 0x02;

    controlPort.outb(tmp);

    if(controlPort.inb() == tmp) {
        controlPort.outb(static_cast<uint8_t>(tmp ^ 0x02));

        return true;
    }

    return false;
}

template<LptPort port>
ParallelDriver<port>::ParallelDriver(ParallelMode mode) : mode(mode),
                                                          sppDataPort(port),
                                                          sppStatusPort(static_cast<uint16_t>(port + 1)),
                                                          sppControlPort(static_cast<uint16_t>(port + 2)),
                                                          eppAddressPort(static_cast<uint16_t>(port + 3)),
                                                          eppDataPort(static_cast<uint16_t>(port + 4)) {
    initializePort();

    timeService = Kernel::getService<TimeService>();
}

template<LptPort port>
void ParallelDriver<port>::setMode(ParallelMode mode) {
    this->mode = mode;

    initializePort();
}

template<LptPort port>
ParallelMode ParallelDriver<port>::getMode() {
    return mode;
}

template<LptPort port>
void ParallelDriver<port>::initializePort() {
    uint8_t control = sppControlPort.inb();

    control = control | static_cast<uint8_t>(0x06); // Initialize Printer and enable automatic linefeed
    control = control & static_cast<uint8_t>(0xce); // Clear strobe-, irq-, and mode-bits

    sppControlPort.outb(control);
}

template<LptPort port>
bool ParallelDriver<port>::isBusy() {
    return (sppStatusPort.inb() & 0x80u) == 0x00; // NOTE: The busy-flag is active low!
}

template<LptPort port>
bool ParallelDriver<port>::isPaperEmpty() {
    return (sppStatusPort.inb() & 0x20u) == 0x20;
}

template<LptPort port>
bool ParallelDriver<port>::checkError() {
    return (sppStatusPort.inb() & 0x08u) == 0x08;
}

template<LptPort port>
void ParallelDriver<port>::sendChar(char c) {
    if (mode == SPP) {
        while (isBusy()); // Wait for the printer to be ready

        sppDataPort.outb(static_cast<uint8_t>(c)); // Send the byte

        // Pulse the strobe bit, so that the printer knows, that there is data to be fetched on the data port.
        uint8_t control = sppControlPort.inb();
        sppControlPort.outb(control | static_cast<uint8_t>(0x01));
        timeService->msleep(10);
        sppControlPort.outb(control);

        while (isBusy()); // Wait for the printer to finish reading the data
    } else if (mode == EPP) {
        eppDataPort.outb(static_cast<uint8_t>(c)); // In EPP-mode, we can just write our byte to the data port.
    }
}

template<LptPort port>
char ParallelDriver<port>::readChar() {
    if (mode == EPP) {
        return eppDataPort.inb();
    }

    return 0;
}

template<LptPort port>
void ParallelDriver<port>::sendData(char *data, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        sendChar(data[i]);

        if (data[i] == '\n') {
            sendChar(13);
        }
    }
}

template<LptPort port>
LptPort ParallelDriver<port>::getPortNumber() {
    return port;
}

template<LptPort port>
String ParallelDriver<port>::getName() {
    switch(port) {
        case LPT1 :
            return NAME_1;
        case LPT2 :
            return NAME_2;
        case LPT3 :
            return NAME_3;
    }
}

}

#endif
