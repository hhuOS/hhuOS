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

#ifndef HHUOS_UDPHEADER_H
#define HHUOS_UDPHEADER_H

#include <cstdint>

namespace Util {
namespace Stream {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Udp {

class UdpHeader {

public:
    /**
     * Default Constructor.
     */
    UdpHeader() = default;

    /**
     * Copy Constructor.
     */
    UdpHeader(const UdpHeader &other) = delete;

    /**
     * Assignment operator.
     */
    UdpHeader &operator=(const UdpHeader &other) = delete;

    /**
     * Destructor.
     */
    ~UdpHeader() = default;

    void read(Util::Stream::InputStream &stream);

    void write(Util::Stream::OutputStream &stream) const;

    [[nodiscard]] uint16_t getSourcePort() const;

    void setSourcePort(uint16_t sourcePort);

    [[nodiscard]] uint16_t getDestinationPort() const;

    void setDestinationPort(uint16_t targetPort);

    [[nodiscard]] uint16_t getDatagramLength() const;

    void setDatagramLength(uint16_t length);

    [[nodiscard]] uint16_t getChecksum() const;

    static const constexpr uint32_t HEADER_SIZE = 8;

private:

    uint16_t sourcePort{};
    uint16_t destinationPort{};
    uint16_t datagramLength{};
    uint16_t checksum{};
};

}

#endif
