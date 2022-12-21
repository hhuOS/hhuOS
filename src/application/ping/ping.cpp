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

#include <cstdint>

#include "lib/util/system/System.h"
#include "lib/util/stream/PrintWriter.h"
#include "lib/util/network/Socket.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/NetworkAddress.h"

int32_t main(int32_t argc, char *argv[]) {
    auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ICMP);
    if (!socket.bind(Util::Network::Ip4::Ip4Address("127.0.0.1"))) {
        Util::System::error << "ping: Failed to bind socket!";
    }

    auto *bindAddress = socket.getLocalAddress();
    Util::System::out << bindAddress->toString() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
    delete bindAddress;

    return 0;
}