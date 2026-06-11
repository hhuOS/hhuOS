/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
 */

#include <stdint.h>

#include <util/base/System.h>
#include <util/io/stream/PrintStream.h>
#include <util/base/ArgumentParser.h>
#include <util/network/ip4/Ip4PortAddress.h>
#include <util/network/Socket.h>
#include <util/network/udp/UdpDatagram.h>
#include <util/base/String.h>
#include <util/io/stream/InputStream.h>

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

constexpr uint16_t DEFAULT_PORT = 1797;

int32_t server(const Util::Network::Socket &socket) {
    Util::Network::Ip4::Ip4PortAddress localAddress;
    if (!socket.getLocalAddress(localAddress)) {
        Util::System::error << "uecho: Failed to query socket address!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    Util::System::out << "UDP echo sever running on " << localAddress.toString() << "! Send 'exit' to leave."
        << Util::Io::PrintStream::lnFlush;

    while (true) {
        Util::Network::Udp::UdpDatagram receiveDatagram;
        if (!socket.receive(receiveDatagram)) {
            Util::System::error << "uecho: Failed to receive echo request!" << Util::Io::PrintStream::lnFlush;
            return -1;
        }

        if (!socket.send(receiveDatagram)) {
            Util::System::error << "uecho: Failed to send echo reply!" << Util::Io::PrintStream::lnFlush;
            return -1;
        }

        const auto datagramString = Util::String(receiveDatagram.getData(), receiveDatagram.getLength()).strip();
        if (datagramString == "exit") {
            break;
        }
    }

    return 0;
}

int32_t client(const Util::Network::Socket &socket, const Util::Network::Ip4::Ip4PortAddress &destinationAddress) {
    auto localAddress = Util::Network::Ip4::Ip4PortAddress();
    if (!socket.getLocalAddress(localAddress)) {
        Util::System::error << "uecho: Failed to query socket address!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    Util::System::out << "UDP echo client running on " << localAddress.toString() << " and sending to "
        << destinationAddress.toString() << "! Type 'exit' to leave." << Util::Io::PrintStream::lnFlush;

    while (true) {
        const auto line = Util::System::in.readLine();
        const auto sendMessage = line.content.strip();
        
        if (sendMessage.isEmpty()) {
            if (line.endOfFile) {
                break;
            }
            
            continue;
        }

        Util::Network::Udp::UdpDatagram sendDatagram(static_cast<const uint8_t*>(sendMessage),
            sendMessage.length(), destinationAddress);
        
        if (!socket.send(sendDatagram)) {
            Util::System::error << "uecho: Failed to send echo request!" << Util::Io::PrintStream::lnFlush;
            return -1;
        }

        Util::Network::Udp::UdpDatagram receiveDatagram;
        if (!socket.receive(receiveDatagram)) {
            Util::System::error << "uecho: Failed to receive echo reply!" << Util::Io::PrintStream::lnFlush;
            return -1;
        }

        auto receiveMessage = Util::String(receiveDatagram.getData(), receiveDatagram.getLength()).strip();
        Util::System::out << "Received: " << sendMessage << Util::Io::PrintStream::lnFlush;

        if (sendMessage == "exit") {
            break;
        }
    }

    return 0;
}

int32_t main(const int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.addSwitch("server", "s");
    argumentParser.addArgument("remote", false, "r");
    argumentParser.addArgument("address", false, "a");
    argumentParser.setHelpText(HELP_TEXT);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    if (!argumentParser.checkSwitch("server") && !argumentParser.hasArgument("remote")) {
        Util::System::error << "uecho: Please specify server/client mode!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    Util::Network::Ip4::Ip4PortAddress bindAddress;
    if (argumentParser.hasArgument("address")) {
        bindAddress = Util::Network::Ip4::Ip4PortAddress(argumentParser.getArgument("address"));
    } else if (argumentParser.checkSwitch("server")) {
        bindAddress = Util::Network::Ip4::Ip4PortAddress(DEFAULT_PORT);
    }

    const Util::Network::Socket socket(Util::Network::Socket::UDP);
    if (!socket.bind(bindAddress)) {
        Util::System::error << "uecho: Failed to bind socket!" << Util::Io::PrintStream::lnFlush;
    }

    if (argumentParser.checkSwitch("server")) {
        return server(socket);
    }

    const Util::Network::Ip4::Ip4PortAddress destinationAddress(argumentParser.getArgument("remote"));
    if (destinationAddress.getPort() == 0) {
        destinationAddress.setPort(DEFAULT_PORT);
    }

    return client(socket, destinationAddress);
}
