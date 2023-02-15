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

#include "lib/util/base/System.h"
#include "lib/util/io/stream/PrintWriter.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/network/ip4/Ip4PortAddress.h"
#include "lib/util/network/Socket.h"
#include "lib/util/network/udp/UdpDatagram.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/InputStream.h"

static const constexpr uint32_t DEFAULT_PORT = 1797;

int32_t server(Util::Network::Socket &socket) {
    auto localAddress = Util::Network::Ip4::Ip4PortAddress();
    if (!socket.getLocalAddress(localAddress)) {
        Util::System::error << "uecho: Failed to query socket address!" << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;
        return -1;
    }

    Util::System::out << "UDP echo sever running on " << localAddress.toString() << "! Send 'exit' to leave." << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;

    while (true) {
        auto receivedDatagram = Util::Network::Udp::UdpDatagram();
        if (!socket.receive(receivedDatagram)) {
            Util::System::error << "uecho: Failed to receive echo request!" << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;
            return -1;
        }

        if (!socket.send(receivedDatagram)) {
            Util::System::error << "uecho: Failed to send echo reply!" << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;
            return -1;
        }

        if (Util::String(receivedDatagram.getData(), receivedDatagram.getLength()) == "exit") {
            break;
        }
    }

    return 0;
}

int32_t client(Util::Network::Socket &socket, const Util::Network::Ip4::Ip4PortAddress &destinationAddress) {
    auto localAddress = Util::Network::Ip4::Ip4PortAddress();
    if (!socket.getLocalAddress(localAddress)) {
        Util::System::error << "uecho: Failed to query socket address!" << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;
        return -1;
    }

    Util::System::out << "UDP echo client running on " << localAddress.toString() << " and sending to " << destinationAddress.toString()
        << "! Type 'exit' to leave." << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;

    while (true) {
        Util::String line;
        char c = Util::System::in.read();
        while (c != '\n') {
            line += c;
            c = Util::System::in.read();
        }

        if (line.isEmpty()) {
            continue;
        }

        auto sendDatagram = Util::Network::Udp::UdpDatagram(static_cast<const uint8_t*>(line), line.length(), destinationAddress);
        if (!socket.send(sendDatagram)) {
            Util::System::error << "uecho: Failed to send echo request!" << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;
            return -1;
        }

        auto receivedDatagram = Util::Network::Udp::UdpDatagram();
        if (!socket.receive(receivedDatagram)) {
            Util::System::error << "uecho: Failed to receive echo reply!" << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;
            return -1;
        }

        auto message = Util::String(receivedDatagram.getData(), receivedDatagram.getLength());
        Util::System::out << "Received: " << message << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;

        if (message == "exit") {
            break;
        }
    }

    return 0;
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.addSwitch("server", "s");
    argumentParser.addArgument("remote", false, "r");
    argumentParser.addArgument("address", false, "a");
    argumentParser.setHelpText("Start an echo server/client.\n"
                               "Usage: uecho [OPTION]...\n"
                               "Options:\n"
                               "  -s, --server: Start echo server\n"
                               "  -r, --remote [ADDRESS]: Start echo client and connect to ADDRESS\n"
                               "  -a, --address [ADDRESS]: Bind socket to ADDRESS (Default: 0.0.0.0:1797)\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;
        return -1;
    }

    if (!argumentParser.checkSwitch("server") && !argumentParser.hasArgument("remote")) {
        Util::System::error << "uecho: Please specify server/client mode!" << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;
        return -1;
    }

    auto bindAddress = Util::Network::Ip4::Ip4PortAddress();
    if (argumentParser.hasArgument("address")) {
        bindAddress = Util::Network::Ip4::Ip4PortAddress(argumentParser.getArgument("address"));
    } else if (argumentParser.checkSwitch("server")) {
        bindAddress = Util::Network::Ip4::Ip4PortAddress(DEFAULT_PORT);
    }

    auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::UDP);
    if (!socket.bind(bindAddress)) {
        Util::System::error << "uecho: Failed to bind socket!" << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;
    }

    if (argumentParser.checkSwitch("server")) {
        return server(socket);
    } else {
        auto destinationAddress = Util::Network::Ip4::Ip4PortAddress(argumentParser.getArgument("remote"));
        if (destinationAddress.getPort() == 0) {
            destinationAddress.setPort(DEFAULT_PORT);
        }

        return client(socket, destinationAddress);
    }
}