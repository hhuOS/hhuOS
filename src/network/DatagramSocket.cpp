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
 */

#include "DatagramSocket.h"

#include "lib/util/async/Thread.h"
#include "lib/util/network/NetworkAddress.h"

namespace Network {
class Datagram;
}  // namespace Network

Network::Datagram* Network::DatagramSocket::receive() {
    while (incomingDatagramQueue.isEmpty()) {
        Util::Async::Thread::yield();
    }

    lock.acquire();
    auto *datagram = incomingDatagramQueue.poll();
    lock.release();

    return datagram;
}

void Network::DatagramSocket::handleIncomingDatagram(Network::Datagram *datagram) {
    lock.acquire();
    incomingDatagramQueue.offer(datagram);
    lock.release();
}

Util::Memory::String Network::DatagramSocket::getName() {
    return bindAddress->toString();
}

Util::File::Type Network::DatagramSocket::getFileType() {
    return Util::File::CHARACTER;
}

uint64_t Network::DatagramSocket::getLength() {
    return 0;
}

Util::Data::Array<Util::Memory::String> Network::DatagramSocket::getChildren() {
    return Util::Data::Array<Util::Memory::String>(0);
}

uint64_t Network::DatagramSocket::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    return 0;
}

uint64_t Network::DatagramSocket::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    return 0;
}
