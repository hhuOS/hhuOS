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

#ifndef HHUOS_ECHOHEADER_H
#define HHUOS_ECHOHEADER_H

#include <cstdint>

namespace Util {
namespace Stream {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Icmp {

class EchoHeader {

public:
    /**
     * Default Constructor.
     */
    EchoHeader() = default;

    /**
     * Copy Constructor.
     */
    EchoHeader(const EchoHeader &other) = delete;

    /**
     * Assignment operator.
     */
    EchoHeader &operator=(const EchoHeader &other) = delete;

    /**
     * Destructor.
     */
    ~EchoHeader() = default;

    void read(Util::Stream::InputStream &stream);

    void write(Util::Stream::OutputStream &stream) const;

    [[nodiscard]] uint16_t getIdentifier() const;

    void setIdentifier(uint16_t identifier);

    [[nodiscard]] uint16_t getSequenceNumber() const;

    void setSequenceNumber(uint16_t sequenceNumber);

    static const constexpr uint32_t HEADER_LENGTH = 4;

private:

    uint16_t identifier{};
    uint16_t sequenceNumber{};
};

}

#endif
