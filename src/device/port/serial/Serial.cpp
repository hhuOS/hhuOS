/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#include "Serial.h"

#include "lib/util/base/Exception.h"

namespace Device::Serial {

ComPort portFromString(const Util::String &portName) {
    const auto port = portName.toLowerCase();

    if (port == "com1") {
        return Serial::COM1;
    } else if (port == "com2") {
        return Serial::COM2;
    } else if (port == "com3") {
        return Serial::COM3;
    } else if (port == "com4") {
        return Serial::COM4;
    } else {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "COM: Invalid port!");
    }
}

const char* portToString(const ComPort port) {
    switch (port) {
        case COM1:
            return "COM1";
        case COM2:
            return "COM2";
        case COM3:
            return "COM3";
        case COM4:
            return "COM4";
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "COM: Invalid port!");
    }
}

}