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

#ifndef HHUOS_ISABUGGER_H
#define HHUOS_ISABUGGER_H

#include <cstdint>

namespace Device {
class IoPort;

class IsaBugger {

public:
    /**
     * Default Constructor.
     */
    IsaBugger() = default;

    /**
     * Copy Constructor.
     */
    IsaBugger(const IsaBugger &other) = delete;

    /**
     * Assignment operator.
     */
    IsaBugger &operator=(const IsaBugger &other) = delete;

    /**
     * Destructor.
     */
    ~IsaBugger() = default;

    static void setLeftDisplay(uint8_t value);

    static void setRightDisplay(uint8_t value);

    static void setRedLeds(uint8_t value);

    static void setGreenLeds(uint8_t value);

private:

    enum Register {
        RED_LEDS = 0x00,
        GREEN_LEDS = 0x01,
        RIGHT_DISPLAY = 0x02,
        LEFT_DISPLAY = 0x04,
        SERIAL_PORT_DATA = 0x20,
        SERIAL_PORT_CONFIGURATION = 0x40,
        INITIALIZE = 0x80,
        RESET = 0xff
    };

    static void setValue(Register reg, uint8_t value);

    static IoPort indexPort;
    static IoPort dataPort;
};

}

#endif
