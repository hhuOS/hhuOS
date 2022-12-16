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

#include <cstdint>

namespace Device {
class IoPort;

/**
 * A simple class to make working with the CMOS more easy.
 */
class Cmos {

public:
    /**
     * Read a CMOS-Register.
     *
     * @param registerIndex The register to read from (0-127)
     *
     * @return The read value
     */
    static uint8_t read(uint8_t registerIndex);

    /**
     * Write to a CMOS-Register.
     *
     * @param registerIndex The register to write to (0-127)
     * @param value The value to write
     */
    static void write(uint8_t registerIndex, uint8_t value);

    /**
     * Disable non-maskable interrupts.
     */
    static void disableNmi();

    /**
     * Enable non-maskable interrupts.
     */
    static void enableNmi();

private:

    static IoPort registerPort;
    static IoPort dataPort;

    /**
     * Keeps track of how often disableNmi() and enableNmi() have been called.
     * Non-maskable interrupts stay disabled, as long as this number is greater than zero.
     */
    static int32_t nmiCount;

};

}

#endif