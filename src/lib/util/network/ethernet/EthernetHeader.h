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

#ifndef HHUOS_ETHERNETHEADER_H
#define HHUOS_ETHERNETHEADER_H

#include <cstdint>

#include "lib/util/network/MacAddress.h"

namespace Util {
namespace Stream {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Ethernet {

class EthernetHeader {

public:
    //Relevant EtherTypes -> list available in RFC7042 Appendix B (pages 25,26)
    enum EtherType : uint16_t {
        IP4 = 0x0800,
        ARP = 0x0806,
        IP6 = 0x86dd
    };

    /**
     * Default Constructor.
     */
    EthernetHeader() = default;

    /**
     * Copy Constructor.
     */
    EthernetHeader(const EthernetHeader &other) = delete;

    /**
     * Assignment operator.
     */
    EthernetHeader &operator=(const EthernetHeader &other) = delete;

    /**
     * Destructor.
     */
    ~EthernetHeader() = default;

    void read(Util::Stream::InputStream &stream);

    void write(Util::Stream::OutputStream &stream);

    [[nodiscard]] const Util::Network::MacAddress& getDestinationAddress() const;

    [[nodiscard]] const Util::Network::MacAddress& getSourceAddress() const;

    [[nodiscard]] EtherType getEtherType() const;

    void setDestinationAddress(const Util::Network::MacAddress &address);

    void setSourceAddress(const Util::Network::MacAddress &address);

    void setEtherType(EtherType type);

    static const constexpr uint32_t HEADER_LENGTH = 14;

private:

    Util::Network::MacAddress destinationAddress{};
    Util::Network::MacAddress sourceAddress{};
    EtherType etherType{};
};

}

#endif
