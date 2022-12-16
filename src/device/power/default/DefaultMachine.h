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

#ifndef HHUOS_DEFAULTMACHINE_H
#define HHUOS_DEFAULTMACHINE_H

#include <cstdint>

#include "device/cpu/IoPort.h"
#include "device/power/Machine.h"

namespace Device {

class DefaultMachine : public Machine {

public:
    /**
     * Default Constructor.
     */
    DefaultMachine();

    /**
     * Copy Constructor.
     */
    DefaultMachine(const DefaultMachine &other) = delete;

    /**
     * Assignment operator.
     */
    DefaultMachine &operator=(const DefaultMachine &other) = delete;

    /**
     * Destructor.
     */
    ~DefaultMachine() override = default;

    void shutdown() override;

    void reboot() override;

private:

    const IoPort qemuShutdownPort = IoPort(0x501);
    const IoPort keyboardControlPort = IoPort(0x64);

    static const constexpr uint16_t CPU_RESET_CODE = 0xfe;
};

}

#endif
