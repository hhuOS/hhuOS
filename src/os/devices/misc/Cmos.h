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

#ifndef HHUOS_CMOS_H
#define HHUOS_CMOS_H

#include <devices/cpu/IOport.h>

/**
 * A simple class to make working with the CMOS mor easy.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class Cmos {

public:

    /**
     * Returns the result of checkAvailability().
     *
     * If the result is false, there is something wrong with the CMOS and its data should not be trusted!
     */
    static bool isAvailable();

    /**
     * Read a CMOS-Register.
     *
     * @param regNumber The register to write to (0-128)
     *
     * @return The read value
     */
    static uint8_t readRegister(uint8_t regNumber);

    /**
     * Write to a CMOS-Register.
     *
     * @param regNumber The register to read from (0-128)
     * @param value The value to write
     */
    static void writeRegister(uint8_t regNumber, uint8_t value);

    /**
     * Disable non-maskable interrupts.
     */
    static void disableNmi();

    /**
     * Enable non-maskable interrupts.
     */
    static void enableNmi();

private:

    /**
     * Perform a simple check, by flipping two bits in RTC's Status Register B.
     *
     * A more sophisticated method of checking the CMOS would be to calculate the checksum and check it against
     * the bytes in registers 0x2e and 0x2f. However, not all BIOSes store the checksum in these registers
     * and even QEMU does not seem to do so, as both registers always contain zeroes, when running in QEMU.
     *
     * @return true, if the check was succesful
     */
    static bool checkAvailability() noexcept;

private:

    static bool available;

    static IOport registerPort;

    static IOport dataPort;

};

#endif
