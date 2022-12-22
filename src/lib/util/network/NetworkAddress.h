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

#ifndef HHUOS_NETWORKADDRESS_H
#define HHUOS_NETWORKADDRESS_H

#include <cstdint>

#include "lib/util/memory/String.h"

namespace Util {
namespace Stream {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network {

class NetworkAddress {

public:

    enum Type {
        MAC, IP4, IP6, IP4_PORT, IP6_PORT
    };

    /**
     * Constructor.
     */
    NetworkAddress(uint8_t length, Type type);

    /**
     * Constructor.
     */
    NetworkAddress(const uint8_t *buffer, uint8_t length, Type type);

    /**
     * Copy Constructor.
     */
    NetworkAddress(const NetworkAddress &other);

    /**
     * Assignment operator.
     */
    NetworkAddress &operator=(const NetworkAddress &other);

    /**
     * Destructor.
     */
    virtual ~NetworkAddress();

    bool operator==(const NetworkAddress &other) const;

    bool operator!=(const NetworkAddress &other) const;

    void read(Util::Stream::InputStream &stream);

    void write(Util::Stream::OutputStream &stream) const;

    void setAddress(const uint8_t *buffer);

    void setAddress(const NetworkAddress &other);

    void getAddress(uint8_t *buffer) const;

    [[nodiscard]] uint8_t getLength() const;

    [[nodiscard]] Type getType() const;

    [[nodiscard]] uint8_t compareTo(const NetworkAddress &other) const;

    [[nodiscard]] virtual NetworkAddress* createCopy() const = 0;

    virtual void setAddress(const Util::Memory::String &string) = 0;

    [[nodiscard]] virtual Util::Memory::String toString() const = 0;

protected:

    uint8_t *buffer{};
    uint8_t length;

private:

    Type type;
};

}

#endif
