/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil,  Michael Schoettner
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

#ifndef HHUOS_IP4ADDRESS_H
#define HHUOS_IP4ADDRESS_H

#include "lib/util/stream/InputStream.h"

namespace Network::Ip4 {

class Ip4Address {

public:

    static const constexpr uint8_t ADDRESS_LENGTH = 4;

    struct Address {
        uint8_t buffer[ADDRESS_LENGTH]{};
    };

    /**
     * Default Constructor.
     */
    Ip4Address() = default;

    /**
     * Constructor.
     */
    explicit Ip4Address(uint8_t *buffer);

    /**
     * Copy Constructor.
     */
    Ip4Address(const Ip4Address &other) = default;

    /**
     * Assignment operator.
     */
    Ip4Address &operator=(const Ip4Address &other) = default;

    /**
     * Destructor.
     */
    ~Ip4Address() = default;

    Address getAddress();

    void setAddress(uint8_t *buffer);

    void readAddress(Util::Stream::InputStream &stream);

private:

    Address address;
};

}

#endif
