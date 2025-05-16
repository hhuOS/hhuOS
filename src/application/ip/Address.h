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

#ifndef HHUOS_ADDRESS_H
#define HHUOS_ADDRESS_H

#include <stdint.h>

#include "lib/util/collection/Array.h"
#include "lib/util/base/String.h"

class Address {

public:
    /**
     * Constructor.
     */
    explicit Address(const Util::Array<Util::String> &arguments);

    /**
     * Copy Constructor.
     */
    Address(const Address &other) = delete;

    /**
     * Assignment operator.
     */
    Address &operator=(const Address &other) = delete;

    /**
     * Destructor.
     */
    ~Address() = default;

    int32_t parse();

    int32_t show();

    int32_t remove();

    int32_t add();

    static const constexpr char *COMMAND = "address";

private:

    static void printDeviceInfo(const Util::String &deviceName);

    const Util::Array<Util::String> arguments;

    static const constexpr char *COMMAND_SHOW = "show";
    static const constexpr char *COMMAND_REMOVE = "delete";
    static const constexpr char *COMMAND_ADD = "add";
};


#endif
