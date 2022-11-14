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

#ifndef HHUOS_ETHERNETFRAME_H
#define HHUOS_ETHERNETFRAME_H

#include "lib/util/stream/InputStream.h"
#include "network/MacAddress.h"
#include "kernel/log/Logger.h"

namespace Network::Ethernet {

class EthernetFrame {

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
    EthernetFrame() = default;

    /**
     * Copy Constructor.
     */
    EthernetFrame(const EthernetFrame &other) = delete;

    /**
     * Assignment operator.
     */
    EthernetFrame &operator=(const EthernetFrame &other) = delete;

    /**
     * Destructor.
     */
    ~EthernetFrame() = default;

    void read(Util::Stream::InputStream &stream);

    [[nodiscard]] MacAddress getDestinationAddress() const;

    [[nodiscard]] MacAddress getSourceAddress() const;

    [[nodiscard]] EtherType getEtherType() const;

    [[nodiscard]] uint32_t getCheckSequence() const;

private:

    MacAddress destinationAddress{};
    MacAddress sourceAddress{};
    EtherType etherType{};
    uint32_t checkSequence = 0;

    static Kernel::Logger log;
};

}

#endif
