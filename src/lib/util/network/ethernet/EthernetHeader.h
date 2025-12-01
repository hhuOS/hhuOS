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

#ifndef HHUOS_LIB_UTIL_NETWORK_ETHERNETHEADER_H
#define HHUOS_LIB_UTIL_NETWORK_ETHERNETHEADER_H

#include <stdint.h>

#include "util/network/MacAddress.h"

namespace Util {
namespace Io {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Ethernet {

/// Represents the header of an Ethernet frame.
/// An Ethernet header contains the destination and source MAC addresses, as well as the EtherType field.
/// The header is 14 bytes long and consists of the following fields:
///
/// | 6 bytes         | 6 bytes    | 2 bytes   |
/// |-----------------|------------|-----------|
/// | Destination MAC | Source MAC | EtherType |
class EthernetHeader {

public:
    /// Relevant EtherTypes (full list is available in RFC7042 Appendix B (pages 25,26))
    enum EtherType : uint16_t {
        /// Used to indicate that the ether type field is invalid.
        INVALID = 0x0000,
        /// IPv4 (Internet Protocol version 4)
        IP4 = 0x0800,
        /// ARP (Address Resolution Protocol)
        ARP = 0x0806,
        /// IPv6 (Internet Protocol version 6)
        IP6 = 0x86dd
    };

    /// Create an empty Ethernet header.
    /// The destination and source MAC addresses are initialized to "00:00:00:00:00:00"
    /// and the EtherType is set to `INVALID`.
    EthernetHeader() = default;

    /// Read the header values from the given input stream.
    /// The input stream must deliver the 14 bytes of data, that make up the Ethernet header.
    /// Destination and source MAC addresses and the EtherType field are read in this exact order.
    void read(Io::InputStream &stream);

    /// Write the header values to the given output stream.
    /// The output stream will receive the 14 bytes of data, that make up the Ethernet header.
    /// Destination and source MAC addresses and the EtherType field are written in this exact order.
    void write(Io::OutputStream &stream) const;

    /// Get the destination MAC address.
    const MacAddress& getDestinationAddress() const;

    /// Get the source MAC address.
    const MacAddress& getSourceAddress() const;

    /// Get the EtherType.
    EtherType getEtherType() const;

    /// Set the destination MAC address.
    void setDestinationAddress(const MacAddress &address);

    /// Set the source MAC address.
    void setSourceAddress(const MacAddress &address);

    /// Set the EtherType.
    void setEtherType(EtherType type);

    /// The length of the Ethernet header in bytes.
    static constexpr uint32_t HEADER_LENGTH = 14;

private:

    MacAddress destinationAddress;
    MacAddress sourceAddress;
    EtherType etherType = INVALID;
};

}

#endif
