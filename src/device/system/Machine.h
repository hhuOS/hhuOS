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

#ifndef HHUOS_MACHINE_H
#define HHUOS_MACHINE_H

#include <stdint.h>

#include "device/cpu/IoPort.h"

namespace Device {

class Machine {

public:
    /**
     * Default Constructor.
     */
    Machine() = default;

    /**
     * Copy Constructor.
     */
    Machine(const Machine &other) = delete;

    /**
     * Assignment operator.
     */
    Machine &operator=(const Machine &other) = delete;

    /**
     * Destructor.
     */
    virtual ~Machine() = default;

    virtual void shutdown();

    virtual void reboot();

private:

    const IoPort qemuShutdownPort = IoPort(0x501);
    const IoPort keyboardControlPort = IoPort(0x64);

    static const constexpr uint16_t CPU_RESET_CODE = 0xfe;
};

}

#endif