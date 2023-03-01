/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Loopback.h"
#include "kernel/network/ethernet/EthernetModule.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"
#include "lib/util/network/NumberUtil.h"

namespace Device::Network {

Util::Network::MacAddress Loopback::getMacAddress() const {
    return {};
}

void Loopback::handleOutgoingPacket(const uint8_t *packet, uint32_t length) {
    Util::Io::ByteArrayOutputStream stream;
    stream.write(packet, 0, length);

    auto checkSequence = Kernel::Network::Ethernet::EthernetModule::calculateCheckSequence(packet, length);
    Util::Network::NumberUtil::writeUnsigned32BitValue(checkSequence, stream);

    handleIncomingPacket(stream.getBuffer(), stream.getLength());
    freeLastSendBuffer();
}

}