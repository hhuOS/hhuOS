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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_MACADDRESS_H
#define HHUOS_MACADDRESS_H

#include <stdint.h>

#include "NetworkAddress.h"
#include "lib/util/base/String.h"

namespace Util::Network {

class MacAddress : public NetworkAddress {

public:

    static const constexpr uint8_t ADDRESS_LENGTH = 6;

    /**
     * Default Constructor.
     */
    MacAddress();

    /**
     * Constructor.
     */
    explicit MacAddress(const uint8_t *buffer);

    /**
     * Constructor.
     */
    explicit MacAddress(const Util::String &string);

    /**
     * Copy Constructor.
     */
    MacAddress(const MacAddress &other) = default;

    /**
     * Assignment operator.
     */
    MacAddress &operator=(const MacAddress &other) = default;

    /**
     * Destructor.
     */
    ~MacAddress() override = default;

    static MacAddress createBroadcastAddress();

    [[nodiscard]] bool isBroadcastAddress() const;

    [[nodiscard]] NetworkAddress* createCopy() const override;

    [[nodiscard]] Util::String toString() const override;
};

}

#endif
