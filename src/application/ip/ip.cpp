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
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/network/Socket.h"
#include "lib/util/network/MacAddress.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/network/ip4/Ip4NetworkMask.h"

void printDeviceInfo(const Util::String &deviceName) {
    auto macFile = Util::Io::File("/device/" + deviceName + "/mac");
    if (!macFile.exists()) {
        Util::System::error << "ip: Device '" << deviceName << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }

    auto macStream = Util::Io::FileInputStream(macFile);
    auto macAddress = Util::Network::MacAddress(macStream.readLine());

    auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ETHERNET);
    if (!socket.bind(macAddress)) {
        Util::System::error << "ip: Unable to bind ethernet socket to device '" << deviceName << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }

    auto ipAddress = Util::Network::Ip4::Ip4Address();
    auto mask = Util::Network::Ip4::Ip4NetworkMask();
    bool ip4 = socket.getIp4Address(ipAddress, mask);

    Util::System::out << deviceName << ":" << Util::Io::PrintStream::endl
                      << "    MAC: " << macAddress.toString() << Util::Io::PrintStream::endl;
    if (ip4) Util::System::out << "    IPv4: " << ipAddress.toString() << "/" << mask.getBitCount() << Util::Io::PrintStream::endl;
    Util::System::out << Util::Io::PrintStream::flush;
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.addArgument("remove", false, "r");
    argumentParser.addArgument("set", false, "s");
    argumentParser.setHelpText("Print IP addresses of network devices.\n"
                               "Usage: hexdump [OPTIONS...] [DEVICE...]\n"
                               "Options:\n"
                               "  -r, --remove: Remove the IPv4 address of a given device"
                               "  -s, --set: Set the IPv4 address of a given device"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = Util::ArrayList<Util::String>(argumentParser.getUnnamedArguments());

    if (argumentParser.hasArgument("remove")) {
        if (arguments.size() == 0) {
            Util::System::error << "ip: No device given!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }

        auto deviceName = arguments.get(0);
        auto macFile = Util::Io::File("/device/" + deviceName + "/mac");
        if (!macFile.exists()) {
            Util::System::error << "ip: Device '" << deviceName << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }

        auto macStream = Util::Io::FileInputStream(macFile);
        auto macAddress = Util::Network::MacAddress(macStream.readLine());

        auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ETHERNET);
        if (!socket.bind(macAddress)) {
            Util::System::error << "ip: Unable to bind ethernet socket to device '" << deviceName << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }

        auto ipSplit = argumentParser.getArgument("remove").split("/");
        auto ipAddress = Util::Network::Ip4::Ip4Address(ipSplit[0]);
        auto mask = Util::Network::Ip4::Ip4NetworkMask(ipSplit.length() > 1 ? Util::String::parseInt(ipSplit[1]) : 32);

        if (!socket.removeIp4Address(ipAddress, mask)) {
            Util::System::error << "ip: Failed to remove IPv4 address '" << ipAddress.toString() << "/" << mask.getBitCount() << "' from device '" << deviceName << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        }

        return 0;
    }

    if (argumentParser.hasArgument("set")) {
        if (arguments.size() == 0) {
            Util::System::error << "ip: No device given!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }

        auto deviceName = arguments.get(0);
        auto macFile = Util::Io::File("/device/" + deviceName + "/mac");
        if (!macFile.exists()) {
            Util::System::error << "ip: Device '" << deviceName << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }

        auto macStream = Util::Io::FileInputStream(macFile);
        auto macAddress = Util::Network::MacAddress(macStream.readLine());

        auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ETHERNET);
        if (!socket.bind(macAddress)) {
            Util::System::error << "ip: Unable to bind ethernet socket to device '" << deviceName << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }

        auto ipSplit = argumentParser.getArgument("set").split("/");
        auto ipAddress = Util::Network::Ip4::Ip4Address(ipSplit[0]);
        auto mask = Util::Network::Ip4::Ip4NetworkMask(ipSplit.length() > 1 ? Util::String::parseInt(ipSplit[1]) : 32);

        if (!socket.setIp4Address(ipAddress, mask)) {
            Util::System::error << "ip: Failed to set IPv4 address '" << ipAddress.toString() << "/" << mask.getBitCount() << "' from device '" << deviceName << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        }

        return 0;
    }

    if (arguments.size() == 0) {
        for (const auto &file: Util::Io::File("/device").getChildren()) {
            if (file.beginsWith("eth") || file.beginsWith("loopback")) {
                arguments.add(file);
            }
        }
    }

    for (const auto &deviceName : arguments) {
        printDeviceInfo(deviceName);
    }

    return 0;
}