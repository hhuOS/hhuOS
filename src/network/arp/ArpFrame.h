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

#ifndef HHUOS_ARPFRAME_H
#define HHUOS_ARPFRAME_H

#include "lib/util/stream/InputStream.h"
#include "network/MacAddress.h"
#include "network/ip4/Ip4Address.h"
#include "kernel/log/Logger.h"

namespace Network::Arp {

class ArpFrame {

public:

    enum HardwareAddressType : uint16_t {
        ETHERNET = 0x0001
    };

    enum ProtocolAddressType : uint16_t {
        IP4 = 0x0800
    };

    enum Operation : uint16_t {
        ARP_REQUEST = 0x0001,
        ARP_ANSWER = 0x0002
    };

    /**
     * Default Constructor.
     */
    ArpFrame() = default;

    /**
     * Copy Constructor.
     */
    ArpFrame(const ArpFrame &other) = delete;

    /**
     * Assignment operator.
     */
    ArpFrame &operator=(const ArpFrame &other) = delete;

    /**
     * Destructor.
     */
    ~ArpFrame() = default;

    void read(Util::Stream::InputStream &stream);

    [[nodiscard]] MacAddress getSourceMacAddress() const;

    [[nodiscard]] MacAddress getTargetMacAddress() const;

    [[nodiscard]] Ip4::Ip4Address getSourceIpAddress() const;

    [[nodiscard]] Ip4::Ip4Address getTargetIpAddress() const;

private:

    static void discardPacket(Util::Stream::InputStream &stream, uint8_t hardwareAddressSize, uint8_t protocolAddressSize);

    Operation operation;
    MacAddress sourceMacAddress;
    MacAddress targetMacAddress;
    Ip4::Ip4Address sourceIpAddress;
    Ip4::Ip4Address targetIpAddress;

    static Kernel::Logger log;
};

}

#endif
