/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_USER_MACHINE_H
#define HHUOS_USER_MACHINE_H

#include <cstdint>

namespace Util::Hardware {

class Machine {

public:

    enum ShutdownType : uint8_t {
        SHUTDOWN = 0x01,
        REBOOT = 0x02
    };

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Machine() = delete;

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
    ~Machine() = default;

    static bool shutdown(ShutdownType type = SHUTDOWN);

    static bool reboot();
};

}

#endif
