/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_POWERMANAGEMENTSERVICE_H
#define HHUOS_POWERMANAGEMENTSERVICE_H

#include <cstdint>

#include "Service.h"
#include "device/cpu/IoPort.h"

namespace Kernel {

class PowerManagementService : public Service {

public:
    /**
     * Default Constructor.
     */
    explicit PowerManagementService();

    /**
     * Copy Constructor.
     */
    PowerManagementService(const PowerManagementService &other) = delete;

    /**
     * Assignment operator.
     */
    PowerManagementService &operator=(const PowerManagementService &other) = delete;

    /**
     * Destructor.
     */
    ~PowerManagementService() override = default;

    void shutdownMachine();

    void rebootMachine();

    static const constexpr uint8_t SERVICE_ID = 3;

private:

    const Device::IoPort qemuShutdownPort = Device::IoPort(0x501);
    const Device::IoPort keyboardControlPort = Device::IoPort(0x64);

    static const constexpr uint16_t CPU_RESET_CODE = 0xfe;
};

}

#endif
