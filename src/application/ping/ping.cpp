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
#include <util/network/Socket.h>
#include <util/network/ip4/Ip4Address.h>
#include <util/network/NetworkAddress.h>
#include <util/network/icmp/EchoHeader.h>
#include <util/io/stream/ByteArrayOutputStream.h>
#include <util/io/stream/NumberUtil.h>
#include <util/time/Timestamp.h>
#include <util/network/icmp/IcmpDatagram.h>
#include <util/base/ArgumentParser.h>
#include <util/async/Thread.h>
#include <util/collection/Array.h>
#include <util/base/String.h>
#include <util/network/icmp/IcmpHeader.h>
#include <util/io/stream/ByteArrayInputStream.h>

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

int32_t main(const int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_TEXT);
    argumentParser.addArgument("count", false, "c");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "ping: No arguments provided!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    // Parse destination IP address and repetition count
    const Util::Network::Ip4::Ip4Address destinationAddress(arguments[0]);
    const auto count = Util::String::parseNumber<int32_t>(
        argumentParser.getArgument("count", "10"));

    // Create ICMP socket with 5 seconds timeout to avoid getting stuck during receive
    const Util::Network::Socket socket(Util::Network::Socket::ICMP);
    socket.setTimeout(Util::Time::Timestamp::ofSeconds(5));

    if (!socket.bind(Util::Network::Ip4::Ip4Address::ANY)) {
        Util::System::error << "ping: Failed to bind socket!" << Util::Io::PrintStream::lnFlush;
    }

    // Header of echo packet to send
    Util::Network::Icmp::EchoHeader echoHeader;

    for (int32_t i = 0; i < count; i++) {
        bool validReply = false;

        // Prepare echo header with current sequence number
        echoHeader.setIdentifier(0);
        echoHeader.setSequenceNumber(i);

        // Build network packet from echo header and write current timestamp as payload
        Util::Io::ByteArrayOutputStream packet;
        echoHeader.write(packet);
        Util::Io::NumberUtil::writeUnsigned32BitValue(Util::Time::Timestamp::getSystemTime().toMilliseconds(),
            packet);

        // Create and send ICMP datagram with echo payload
        Util::Network::Icmp::IcmpDatagram datagram(packet, destinationAddress,
            Util::Network::Icmp::IcmpHeader::ECHO_REQUEST, 0);
        if (!socket.send(datagram)) {
            Util::System::error << "ping: Failed to send echo request!" << Util::Io::PrintStream::lnFlush;
            return -1;
        }

        // Read ICMP datagrams until a valid echo reply is received.
        do {
            Util::Network::Icmp::IcmpDatagram receivedDatagram;
            if (!socket.receive(receivedDatagram)) {
                Util::System::error << "ping: Failed to receive echo reply!" << Util::Io::PrintStream::lnFlush;
                return -1;
            }

            // The socket may receive any ICMP datagrams, so we need to filter for ECHO_REPLY
            if (receivedDatagram.getType() == Util::Network::Icmp::IcmpHeader::ECHO_REPLY) {
                Util::Io::ByteArrayInputStream receivedPacket(receivedDatagram.getData(),
                    receivedDatagram.getLength());

                // Read echo header from datagram and check sequence number
                echoHeader.read(receivedPacket);
                if (echoHeader.getSequenceNumber() == i) {
                    validReply = true;
                    const auto sourceTimestamp = Util::Io::NumberUtil::readUnsigned32BitValue(receivedPacket);
                    const auto currentTimestamp = Util::Time::Timestamp::getSystemTime().toMilliseconds();
                    Util::System::out << receivedDatagram.getLength() << " bytes from "
                                      << static_cast<const char*>(receivedDatagram.getRemoteAddress().toString())
                                      << " (Sequence number: " << echoHeader.getSequenceNumber()
                                      << ", Time: " << currentTimestamp - sourceTimestamp << " ms)"
                                      << Util::Io::PrintStream::lnFlush;
                }
            }
        } while (!validReply);

        // Wait one second before sending the next echo datagram
        if (i < count - 1) {
            Util::Async::Thread::sleep(Util::Time::Timestamp(1, 0));
        }
    }

    return 0;
}