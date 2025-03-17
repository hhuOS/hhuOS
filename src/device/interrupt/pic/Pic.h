/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner; Olaf Spinczyk, TU Dortmund
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

#ifndef __PIC_include__
#define __PIC_include__

#include <stdint.h>

#include "device/cpu/IoPort.h"

namespace Device {
enum InterruptRequest : uint8_t;

/**
 * PIC - programmable interrupt controller. In this hardware device the different
 * interrupts can be activated or masked out. Using the PIC one can control,
 * which hardware interrupts shall be passed to the CPU.
 *
 * @author  original by Olaf Spinczyk, TU Dortmund
 *          modified by Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */
class Pic {

public:
    /**
     * Default-Constructor.
     */
    Pic();

    /**
     * Copy Constructor.
     */
    Pic(const Pic &copy) = delete;

    /**
     * Assignment operator.
     */
    Pic &operator=(const Pic &other) = delete;

    /**
     * Destructor.
     */
    ~Pic() = default;

    /**
     * Unmask an interrupt number in the corresponding PIC. If this is done,
     * all interrupts with this number will be passed to the CPU.
     *
     * @param interrupt The number of the interrupt to activated
     */
    void allow(InterruptRequest interrupt);

    /**
     * Forbid an interrupt. If this is done, the interrupt is masked out
     * and every interrupt with this number that is thrown will be
     * suppressed and not arrive the CPU.
     *
     * @param interrupt The number of the interrupt to deactivate
     */
    void forbid(InterruptRequest interrupt);

    /**
     * Get the state of this interrupt - whether it is masked out or not.
     *
     * @param interrupt The number of the interrupt
     * @return true, if the interrupt is disabled
     */
    bool status(InterruptRequest interrupt);

    /**
     * Send an end of interrupt signal to the corresponding PIC.
     *
     * @param interrupt The number of the interrupt for which to send an EOI
     */
    void sendEndOfInterrupt(InterruptRequest interrupt);

    /**
     * Check if a spurious interrupt has occurred.
     *
     * @return true, if a spurious interrupt has occurred
     */
    bool isSpurious(InterruptRequest interrupt);

private:

    enum Command {
        READ_ISR = 0x0b,
        INITIALIZE = 0x11,
        END_OF_INTERRUPT = 0x20
    };

    /**
     * Get the PIC's data port for the specified interrupt.
     *
     * @param interrupt The interrupt
     * @return The corresponding PIC's data port
     */
    const IoPort& getDataPort(InterruptRequest interrupt);

    /**
     * Get the mask for the specified interrupt.
     *
     * @param interrupt The interrupt
     * @return The interrupt's mask
     */
    static uint8_t getMask(InterruptRequest interrupt);

    static void delay();

private:

    const IoPort masterCommandPort = IoPort(0x20);
    const IoPort masterDataPort = IoPort(0x21);

    const IoPort slaveCommandPort = IoPort(0xA0);
    const IoPort slaveDataPort = IoPort(0xA1);

    static const constexpr uint8_t SPURIOUS_INTERRUPT = 0x80;
};

}

#endif
