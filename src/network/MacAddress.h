/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#ifndef HHUOS_MACADDRESS_H
#define HHUOS_MACADDRESS_H

#include <cstdint>
#include "lib/util/stream/InputStream.h"
#include "lib/util/stream/OutputStream.h"

namespace Network {

class MacAddress {

public:

    static const constexpr uint8_t ADDRESS_LENGTH = 6;

    struct Address {
        uint8_t buffer[ADDRESS_LENGTH]{};
    };

    /**
     * Default Constructor.
     */
    MacAddress() = default;

    /**
     * Constructor.
     */
    explicit MacAddress(uint8_t *buffer);

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
    ~MacAddress() = default;

    static MacAddress createBroadcastAddress();

    [[nodiscard]] Address getAddress() const;

    void setAddress(uint8_t *buffer);

    void read(Util::Stream::InputStream &stream);

    void write(Util::Stream::OutputStream &stream) const;

    [[nodiscard]] bool isBroadcastAddress() const;

    bool operator!=(const MacAddress &other) const;

    bool operator==(const MacAddress &other) const;

private:

    Address address;
};

}

#endif
