/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_INTERRUPTREQUEST_H
#define HHUOS_INTERRUPTREQUEST_H

namespace Device {

/**
 * This allows addressing an interrupt line by device name.
 *
 * InterruptRequests map 1:1 to PIC interrupt inputs, and to system InterruptVectors translated by 32.
 * They do not translate 1:1 to GlobalSystemInterrupts.
 */
enum InterruptRequest : uint8_t {
    // PIC compatible devices
    PIT = 0x00,
    KEYBOARD = 0x01,
    CASCADE = 0x02,
    COM2 = 0x03,
    COM1 = 0x04,
    LPT2 = 0x05,
    FLOPPY = 0x06,
    LPT1 = 0x07,
    RTC = 0x08,
    FREE1 = 0x09,
    FREE2 = 0x0a,
    FREE3 = 0x0b,
    MOUSE = 0x0c,
    FPU = 0x0d,
    PRIMARY_ATA = 0x0e,
    SECONDARY_ATA = 0x0f,
    // Other devices
};

}

#endif
