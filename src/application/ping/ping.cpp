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
 */

#include <stdint.h>

#include "lib/util/base/System.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/network/Socket.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/NetworkAddress.h"
#include "lib/util/network/icmp/EchoHeader.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"
#include "../../lib/util/io/stream/NumberUtil.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/network/icmp/IcmpDatagram.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/async/Thread.h"
#include "lib/util/collection/Array.h"
#include "lib/util/base/String.h"
#include "lib/util/network/icmp/IcmpHeader.h"
#include "lib/util/io/stream/ByteArrayInputStream.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.addArgument("count", false, "c");
    argumentParser.setHelpText("Send ICMP echo requests to a remote host and measure the time it takes to receive a response.\n"
                               "Usage: ping [OPTION]... [DESTINATION]\n"
                               "Options:\n"
                               "  -c, --count: The amount of messages to send/receive (Default: 10)\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "ping: No arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    uint32_t count = argumentParser.hasArgument("count") ? Util::String::parseNumber<uint32_t>(argumentParser.getArgument("count")) : 10;
    auto destinationAddress = Util::Network::Ip4::Ip4Address(arguments[0]);

    auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ICMP);
    socket.setTimeout(Util::Time::Timestamp::ofSeconds(5));

    if (!socket.bind(Util::Network::Ip4::Ip4Address::ANY)) {
        Util::System::error << "ping: Failed to bind socket!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    }

    auto echoHeader = Util::Network::Icmp::EchoHeader();

    for (uint32_t i = 0; i < count; i++) {
        bool validReply = false;
        auto packet = Util::Io::ByteArrayOutputStream();
        echoHeader.setIdentifier(0);
        echoHeader.setSequenceNumber(i);
        echoHeader.write(packet);
        Util::Io::NumberUtil::writeUnsigned32BitValue(Util::Time::Timestamp::getSystemTime().toMilliseconds(), packet);

        auto datagram = Util::Network::Icmp::IcmpDatagram(packet, destinationAddress, Util::Network::Icmp::IcmpHeader::ECHO_REQUEST, 0);
        if (!socket.send(datagram)) {
            Util::System::error << "ping: Failed to send echo request!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }

        do {
            auto receivedDatagram = Util::Network::Icmp::IcmpDatagram();
            if (!socket.receive(receivedDatagram)) {
                Util::System::error << "ping: Failed to receive echo reply!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
                return -1;
            }

            if (receivedDatagram.getType() == Util::Network::Icmp::IcmpHeader::ECHO_REPLY) {
                auto receivedPacket = Util::Io::ByteArrayInputStream(receivedDatagram.getData(), receivedDatagram.getLength());
                echoHeader.read(receivedPacket);
                if (echoHeader.getSequenceNumber() == i) {
                    validReply = true;
                    auto sourceTimestamp = Util::Io::NumberUtil::readUnsigned32BitValue(receivedPacket);
                    auto currentTimestamp = Util::Time::Timestamp::getSystemTime().toMilliseconds();
                    Util::System::out << receivedDatagram.getLength() << " bytes from " << static_cast<const char*>(receivedDatagram.getRemoteAddress().toString())
                                      << " (Sequence number: " << echoHeader.getSequenceNumber() << ", Time: " << currentTimestamp - sourceTimestamp << " ms)"
                                      << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
                }
            }
        } while (!validReply);

        if (i < count - 1) {
            Util::Async::Thread::sleep(Util::Time::Timestamp(1, 0));
        }
    }

    return 0;
}