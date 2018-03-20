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

// struct to pass parameters to a bios call
struct BIOScall_params {
    uint16_t DS;
    uint16_t ES;
    uint16_t FS;
    uint16_t Flags;
    uint32_t DI;
    uint32_t SI;
    uint32_t BP;
    uint32_t SP;
    uint32_t BX;
    uint32_t DX;
    uint32_t CX;
    uint32_t AX;
} __attribute__((packed));


// pointer to memory-block where bios call parameters should be stored
extern struct BIOScall_params* BC_params;

/**
 * BIOS - provides BIOS-call functions in Protected Mode / Paging - environment
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */
class Bios {
    
private:
    Bios(const Bios &copy);

public:
    Bios() = delete;

    /**
     * Initializes segment for bios call.
     * Builds up a 16-bit code segment manually. The start address
     * of this code segment is in the GDT for bios calls
     */
    static void init();
    
    /**
     * Provides a bios call via software interrupt
     *
     * @param inter Number of the bios-call
     */
    static void Int(int inter);
};

#endif
