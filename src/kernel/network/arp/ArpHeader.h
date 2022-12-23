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
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_ARPHEADER_H
#define HHUOS_ARPHEADER_H

#include <cstdint>

#include "lib/util/network/MacAddress.h"
#include "lib/util/network/ip4/Ip4Address.h"

namespace Util {
namespace Stream {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel::Network::Arp {

class ArpHeader {

public:

    enum HardwareAddressType : uint16_t {
        ETHERNET = 0x0001
    };

    enum ProtocolAddressType : uint16_t {
        IP4 = 0x0800
    };

    enum Operation : uint16_t {
        REQUEST = 0x0001,
        REPLY = 0x0002
    };

    /**
     * Default Constructor.
     */
    ArpHeader() = default;

    /**
     * Copy Constructor.
     */
    ArpHeader(const ArpHeader &other) = delete;

    /**
     * Assignment operator.
     */
    ArpHeader &operator=(const ArpHeader &other) = delete;

    /**
     * Destructor.
     */
    ~ArpHeader() = default;

    void read(Util::Stream::InputStream &stream);

    void write(Util::Stream::OutputStream &stream);

    [[nodiscard]] HardwareAddressType getHardwareAddressType() const;

    [[nodiscard]] uint8_t getHardwareAddressSize() const;

    [[nodiscard]] ProtocolAddressType getProtocolAddressType() const;

    [[nodiscard]] uint8_t getProtocolAddressSize() const;

    [[nodiscard]] Operation getOperation() const;

    void setOperation(Operation operation);

private:

    HardwareAddressType hardwareAddressType = ETHERNET;
    uint8_t hardwareAddressSize = Util::Network::MacAddress::ADDRESS_LENGTH;

    ProtocolAddressType protocolAddressType = IP4;
    uint8_t protocolAddressSize = Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH;

    Operation operation{};
};

}

#endif
