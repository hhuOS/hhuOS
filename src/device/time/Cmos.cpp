/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <device/cpu/Cpu.h>
#include "Cmos.h"

namespace Device {

bool Cmos::nmiEnabled = true;
IoPort Cmos::registerPort(0x70);
IoPort Cmos::dataPort(0x71);

uint8_t Cmos::read(uint8_t registerIndex) {
    if (registerIndex > 0x7f) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "CMOS: Register index out of bounds!");
    }

    registerPort.writeByte(registerIndex | ((nmiEnabled ? 0 : 1) << 7));
    return dataPort.readByte();
}

void Cmos::write(uint8_t registerIndex, uint8_t value) {
    if (registerIndex > 0x7f) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "CMOS: Register index out of bounds!");
    }

    registerPort.writeByte(registerIndex | ((nmiEnabled ? 0 : 1) << 7));
    dataPort.writeByte(value);
}

void Cmos::disableNmi() {
    uint8_t value = registerPort.readByte() | 0x80; // Set high bit to disable NMI
    registerPort.writeByte(value);
    nmiEnabled = false;
}

void Cmos::enableNmi() {
    uint8_t value = registerPort.readByte() & 0x7f; // Zero out high bit to enable NMI
    registerPort.writeByte(value);
    nmiEnabled = true;
}

}