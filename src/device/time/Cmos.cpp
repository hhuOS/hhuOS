/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Cmos.h"
#include "device/cpu/IoPort.h"
#include "lib/util/Exception.h"
#include "lib/util/async/Atomic.h"

namespace Device {

int32_t Cmos::nmiCount = 1; // Interrupts are disabled on startup
IoPort Cmos::registerPort(0x70);
IoPort Cmos::dataPort(0x71);

uint8_t Cmos::read(uint8_t registerIndex) {
    if (registerIndex > 0x7f) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "CMOS: Register index out of bounds!");
    }

    registerPort.writeByte(registerIndex | ((nmiCount == 0 ? 0 : 1) << 7));
    return dataPort.readByte();
}

void Cmos::write(uint8_t registerIndex, uint8_t value) {
    if (registerIndex > 0x7f) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "CMOS: Register index out of bounds!");
    }

    registerPort.writeByte(registerIndex | ((nmiCount == 0 ? 0 : 1) << 7));
    dataPort.writeByte(value);
}

void Cmos::disableNmi() {
    auto nmiWrapper = Util::Async::Atomic<int32_t>(nmiCount);
    int count = nmiWrapper.fetchAndInc();

    if (count == 0) {
        // nmiCount has been increased from 0 to 1 -> Disable non-maskable interrupts
        uint8_t value = registerPort.readByte() | 0x80; // Set high bit to disable NMI
        registerPort.writeByte(value);
    } else if (count < 0) {
        // nmiCount is negative -> Illegal state
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "CPU: nmiCount is less than 0!");
    }
}

void Cmos::enableNmi() {
    auto nmiWrapper = Util::Async::Atomic<int32_t>(nmiCount);
    int count = nmiWrapper.fetchAndDec();

    if (count == 1) {
        // nmiCount has been decreased to 0 -> Enable non-maskable interrupts
        uint8_t value = registerPort.readByte() & 0x7f; // Zero out high bit to enable NMI
        registerPort.writeByte(value);
    } else if (count < 1) {
        // nmiCount has been decreased to a negative value -> Illegal state
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "CPU: nmiCount is less than 0!");
    }
}

}