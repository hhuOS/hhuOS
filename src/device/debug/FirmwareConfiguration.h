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

#ifndef HHUOS_FIRMWARECONFIGURATION_H
#define HHUOS_FIRMWARECONFIGURATION_H

#include "device/cpu/IoPort.h"

namespace Device {

class FirmwareConfiguration {

public:

    enum Selector : uint16_t {
        SIGNATURE = 0x0000,
        ID = 0x0001
    };

    /**
     * Default Constructor.
     */
    FirmwareConfiguration() = default;

    /**
     * Copy Constructor.
     */
    FirmwareConfiguration(const FirmwareConfiguration &other) = delete;

    /**
     * Assignment operator.
     */
    FirmwareConfiguration &operator=(const FirmwareConfiguration &other) = delete;

    /**
     * Destructor.
     */
    ~FirmwareConfiguration() = default;

    static bool isAvailable();

private:

    IoPort selectorPort = IoPort(0x510);
    IoPort dataPort = IoPort(0x511);
};

}

#endif
