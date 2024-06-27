/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Address.h"

#include "lib/util/io/file/File.h"
#include "lib/util/base/System.h"
#include "lib/util/network/MacAddress.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/network/Socket.h"
#include "lib/util/network/ip4/Ip4SubnetAddress.h"

Address::Address(const Util::Array<Util::String> &arguments) : arguments(arguments) {}

int32_t Address::parse() {
    if (arguments.length() == 0 || Util::String(COMMAND_SHOW).beginsWith(arguments[0])) {
        return show();
    } else if (Util::String(COMMAND_REMOVE).beginsWith(arguments[0])) {
        return remove();
    } else if (Util::String(COMMAND_ADD).beginsWith(arguments[0])) {
        return add();
    }

    Util::System::error << "ip: Invalid argument '" << arguments[0] << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    return -1;
}

int32_t Address::show() {
    if (arguments.length() > 1) {
        printDeviceInfo(arguments[1]);
    } else {
        for (const auto &file: Util::Io::File("/device").getChildren()) {
            if (file.beginsWith("eth") || file.beginsWith("loopback")) {
                printDeviceInfo(file);
            }
        }
    }

    return 0;
}

int32_t Address::remove() {
    if (arguments.length() < 3) {
        Util::System::error << "ip: Missing arguments for 'address " << COMMAND_REMOVE << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return - 1;
    }

    auto ipAddress = Util::Network::Ip4::Ip4SubnetAddress(arguments[1]);
    auto deviceName = Util::String(arguments[2]);

    auto macFile = Util::Io::File("/device/" + deviceName + "/mac");
    if (!macFile.exists()) {
        Util::System::error << "ip: Device '" << deviceName << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    bool endOfFile = false;
    auto macStream = Util::Io::FileInputStream(macFile);
    auto macAddress = Util::Network::MacAddress(macStream.readLine(endOfFile));

    auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ETHERNET);
    if (!socket.bind(macAddress)) {
        Util::System::error << "ip: Unable to bind ethernet socket to device '" << deviceName << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    if (!socket.removeIp4Address(ipAddress)) {
        Util::System::error << "ip: Failed to remove IPv4 address '" << ipAddress.toString() << "' from device '" << deviceName << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    }

    return 0;
}

int32_t Address::add() {
    if (arguments.length() < 3) {
        Util::System::error << "ip: Missing arguments for 'address " << COMMAND_ADD << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return - 1;
    }

    auto ipAddress = Util::Network::Ip4::Ip4SubnetAddress(arguments[1]);
    auto deviceName = Util::String(arguments[2]);

    auto macFile = Util::Io::File("/device/" + deviceName + "/mac");
    if (!macFile.exists()) {
        Util::System::error << "ip: Device '" << deviceName << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    bool endOfFile = false;
    auto macStream = Util::Io::FileInputStream(macFile);
    auto macAddress = Util::Network::MacAddress(macStream.readLine(endOfFile));

    auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ETHERNET);
    if (!socket.bind(macAddress)) {
        Util::System::error << "ip: Unable to bind ethernet socket to device '" << deviceName << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    if (!socket.addIp4Address(ipAddress)) {
        Util::System::error << "ip: Failed to remove IPv4 address '" << ipAddress.toString() << "' from device '" << deviceName << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    }

    return 0;
}

void Address::printDeviceInfo(const Util::String &deviceName) {
    auto macFile = Util::Io::File("/device/" + deviceName + "/mac");
    if (!macFile.exists()) {
        Util::System::error << "ip: Device '" << deviceName << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }

    bool endOfFile = false;
    auto macStream = Util::Io::FileInputStream(macFile);
    auto macAddress = Util::Network::MacAddress(macStream.readLine(endOfFile));

    auto ethernetSocket = Util::Network::Socket::createSocket(Util::Network::Socket::ETHERNET);
    if (!ethernetSocket.bind(macAddress)) {
        Util::System::error << "ip: Unable to bind ethernet socket to device '" << deviceName << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }

    auto ipAddresses = ethernetSocket.getIp4Addresses();

    Util::System::out << deviceName << ":" << Util::Io::PrintStream::endl
                      << "    MAC: " << macAddress.toString() << Util::Io::PrintStream::endl;

    for (const auto &address : ipAddresses) {
        Util::System::out << "    IPv4: " << address.toString() << Util::Io::PrintStream::endl;
    }

    Util::System::out << Util::Io::PrintStream::flush;
}


