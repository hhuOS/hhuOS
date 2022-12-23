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
 *
 * The network stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_NETWORKMASK_H
#define HHUOS_NETWORKMASK_H

#include <cstdint>

namespace Util::Network {

class NetworkMask {

public:
    /**
     * Constructor.
     */
    explicit NetworkMask(uint8_t bitCount);

    /**
     * Copy Constructor.
     */
    NetworkMask(const NetworkMask &other) = default;

    /**
     * Assignment operator.
     */
    NetworkMask &operator=(const NetworkMask &other) = default;

    /**
     * Destructor.
     */
    ~NetworkMask() = default;

    [[nodiscard]] uint8_t getBitCount() const;

protected:

    uint8_t bitCount;
};

}

#endif
