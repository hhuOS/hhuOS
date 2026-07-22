/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_MODELSPECIFICREGISTER_H
#define HHUOS_MODELSPECIFICREGISTER_H

#include <stdint.h>

namespace Device {

class ModelSpecificRegister {

public:
    /**
     * Constructor.
     *
     * @param msr The MSR address as listed in IA-32 manual, sec. 4
     */
    explicit ModelSpecificRegister(const uint32_t address) : address(address) {};

    /**
     * Copy Constructor.
     */
    ModelSpecificRegister(const ModelSpecificRegister &other) = delete;

    /**
     * Assignment operator.
     */
    ModelSpecificRegister &operator=(const ModelSpecificRegister &other) = delete;

    /**
     * Destructor.
     */
    ~ModelSpecificRegister() = default;

    /**
     * Read from a model specific register.
     *
     * @return The read 64 bit value
     */
    uint64_t readQuadWord() const {
        uint32_t low;
        uint32_t high;

        // rdmsr writes read value to eax/edx from register specified in ecx; it has no operands
        asm volatile (
                "rdmsr"
                : "=a"(low), "=d"(high)
                : "c"(address)
                );

        return low | (static_cast<uint64_t>(high) << 32);
    }

    /**
     * Write to a model specific register.
     *
     * @param value The 64 bit value to write
     */
    void writeQuadWord(const uint64_t value) const {
        const uint32_t low = value & 0xFFFFFFFF;
        const uint32_t high = value >> 32;

        // wrmsr writes values from eax/edx to register specified in ecx; it has no operands
        asm volatile (
                "wrmsr"
                :
                : "a"(low), "d"(high), "c"(address)
                );

    }


private:

    uint32_t address;
};

}

#endif
