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
#include "lib/util/network/icmp/EchoHeader.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "lib/util/network/NumberUtil.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/network/icmp/IcmpDatagram.h"
#include "lib/util/ArgumentParser.h"
#include "lib/util/async/Thread.h"
#include "lib/util/data/Array.h"
#include "lib/util/memory/String.h"
#include "lib/util/network/icmp/IcmpHeader.h"
#include "lib/util/stream/ByteArrayInputStream.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.addArgument("count", false, "c");
    argumentParser.setHelpText("Send ICMP echo requests to a remote host and measure the time it takes to receive a response.\n"
                               "Usage: ping [OPTION]... [DESTINATION]\n"
                               "Options:\n"
                               "  -c, --count: The amount of messages to send/receive (Default: 10)\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "ping: No arguments provided!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    uint32_t count = argumentParser.hasArgument("count") ? Util::Memory::String::parseInt(argumentParser.getArgument("count")) : 10;
    auto destinationAddress = Util::Network::Ip4::Ip4Address(arguments[0]);

    auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ICMP);
    if (!socket.bind(Util::Network::Ip4::Ip4Address::ANY)) {
        Util::System::error << "ping: Failed to bind socket!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
    }

    auto echoHeader = Util::Network::Icmp::EchoHeader();

    for (uint32_t i = 0; i < count; i++) {
        bool validReply = false;
        auto packet = Util::Stream::ByteArrayOutputStream();
        echoHeader.setIdentifier(0);
        echoHeader.setSequenceNumber(i);
        echoHeader.write(packet);
        Util::Network::NumberUtil::writeUnsigned32BitValue(Util::Time::getSystemTime().toMilliseconds(), packet);

        auto datagram = Util::Network::Icmp::IcmpDatagram(packet, destinationAddress, Util::Network::Icmp::IcmpHeader::ECHO_REQUEST, 0);
        if (!socket.send(datagram)) {
            Util::System::error << "ping: Failed to send echo request!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
            return -1;
        }

        do {
            auto receivedDatagram = Util::Network::Icmp::IcmpDatagram();
            if (!socket.receive(receivedDatagram)) {
                Util::System::error << "ping: Failed to receive echo reply!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
                return -1;
            }

            if (receivedDatagram.getType() == Util::Network::Icmp::IcmpHeader::ECHO_REPLY) {
                auto receivedPacket = Util::Stream::ByteArrayInputStream(receivedDatagram);
                echoHeader.read(receivedPacket);
                if (echoHeader.getSequenceNumber() == i) {
                    validReply = true;
                    auto sourceTimestamp = Util::Network::NumberUtil::readUnsigned32BitValue(receivedPacket);
                    auto currentTimestamp = Util::Time::getSystemTime().toMilliseconds();
                    Util::System::out << receivedDatagram.getLength() << " bytes from " << static_cast<const char*>(receivedDatagram.getRemoteAddress().toString())
                                    << " (Sequence number: " << echoHeader.getSequenceNumber() << ", Time: " << currentTimestamp - sourceTimestamp << " ms)"
                                    << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
                }
            }
        } while (!validReply);

        Util::Async::Thread::sleep(Util::Time::Timestamp(1, 0));
    }

    return 0;
}