/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University; Olaf Spinczyk, TU Dortmund
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

#include <cstdint>
#include <kernel/IOport.h>

/**
 * PIC - programmable interrupt controller. In this hardware device the different
 * interrupts can be activated or masked out. Using the PIC one can controll,
 * which hardware interrupts shall be passed to the CPU.
 *
 * @author  original by Olaf Spinczyk, TU Dortmund
 * 			modified by Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */
class Pic {

public:

    Pic(const Pic &copy) = delete;

    Pic &operator=(const Pic &other) = delete;

    ~Pic() = default;

    enum class Interrupt : uint8_t {
        PIT             = 0x00,
        KEYBOARD        = 0x01,
        CASCADE         = 0x02,
        COM2            = 0x03,
        COM1            = 0x04,
        LPT2            = 0x05,
        FLOPPY          = 0x06,
        LPT1            = 0x07,
        RTC             = 0x08,
        FREE1           = 0x09,
        FREE2           = 0x0A,
        FREE3           = 0x0B,
        MOUSE           = 0x0C,
        FPU             = 0x0D,
        PRIMARY_ATA     = 0x0E,
        SECONDARY_ATA   = 0x0F
    };

    /**
     * Get or create the singleton instance
     */
    static Pic *getInstance();

    /**
     * Demasks an interrupt number in the corresponding PIC. If this is done,
     * all interrupts with this number will be passed to the CPU.
     *
     * @param interrupt The number of the interrupt that should be activated.
     */
    void allow(Interrupt interrupt);

    /**
     * Forbids an interrupt. If this is done, the interrupt is masked out
     * and every interrupts with this number that is thrown will be
     * surpressed and not arrive the CPU.
     *
     * @param interrupt The number of the interrupt that should be deactivated/masked out.
     */
    void forbid(Interrupt interrupt);

    /**
     * Gets the state of this interrupt - whether it is masked out or not.
     *
     * @param interrupt The number of the interrupt we want to get the state.
     * @return Status of the bit for the interrupt device
     */
    bool status(Interrupt interrupt);

    /**
     * Send an end of interrupt signal to the corresponding PIC.
     *
     * @param interrupt The number of the interrupt for which we want to send an EOI.
     */
    void sendEOI(Interrupt interrupt);

    /**
     * Indicates if a spurious interrupt has occured.
     *
     * @return true, if a spurious interrupt has occured, false else
     */
     static bool isSpurious();

private:

    Pic() = default;

    /**
     * Returns the PIC's data port for the specified interrupt.
     *
     * @param interrupt The interrupt
     * @return The corresponding PIC's data port
     */
    IOport& getDataPort(Interrupt interrupt);

    /**
     * Returns the PIC's comand port for the specified interrupt.
     *
     * @param interrupt The interrupt
     * @return The corresponding PIC's command port
     */
    IOport& getCommandPort(Interrupt interrupt);

    /**
     * Returns the mask for the specified interrupt.
     *
     * @param interrupt The interrupt
     * @return The interrupt's mask
     */
    uint8_t getMask(Interrupt interrupt);

    static Pic *instance;

    static const uint8_t EOI = 0x20;

    static const uint8_t READ_ISR = 0x0B;

    static const uint8_t SPURIOUS_INTERRUPT = 0x80;
};

#endif
