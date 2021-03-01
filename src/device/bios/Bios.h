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

#ifndef __BIOS_include__
#define __BIOS_include__

#include <cstdint>

namespace Device {

/**
 * BIOS - provides BIOS-call functions in Protected Mode / Paging - environment
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */
class Bios {

public:
    // struct to pass parameters to a bios call
    struct CallParameters {
        uint16_t ds;
        uint16_t es;
        uint16_t fs;
        uint16_t flags;
        uint32_t di;
        uint32_t si;
        uint32_t bp;
        uint32_t sp;
        uint32_t bx;
        uint32_t dx;
        uint32_t cx;
        uint32_t ax;
    } __attribute__((packed));

public:
    /**
     * Default-Constructor.
     * Deleted, as this class has only static members.
     */
    Bios() = delete;

    /**
     * Copy constructor.
     */
    Bios(const Bios &other) = delete;

    /**
     * Assignment operator.
     */
    Bios &operator=(const Bios &other) = delete;

    /**
     * Destructor.
     */
    ~Bios() = default;

    /**
     * Check if BIOS-calls are activated.
     * BIOS-calls can be de-/activated with the kernel parameter 'bios_enhancements=true/false'.
     */
    static bool isAvailable();

    /**
     * Initializes segment for bios call.
     * Builds up a 16-bit code segment manually. The start address
     * of this code segment is in the GDT for bios calls
     */
    static void init();

    /**
     * Provides a bios call via software interrupt
     *
     * @param Interrupt number number of the bios call
     * @param callParameters Parameter struct for the bios call
     */
    static void interrupt(int interruptNumber, CallParameters &callParameters);

private:
    // pointer to memory-block where bios call parameters should be stored
    static const CallParameters *parameters;
};

}

#endif
