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

#include "Address.h"

#include <lib/util/io/file/File.h>
#include <lib/util/base/System.h>
#include <lib/util/network/MacAddress.h>
#include <lib/util/io/stream/FileInputStream.h>
#include <lib/util/io/stream/PrintStream.h>
#include <lib/util/network/Socket.h>
#include <lib/util/network/ip4/Ip4SubnetAddress.h>

Address::Address(const Util::Array<Util::String> &arguments) : arguments(arguments) {}

int32_t Address::parse() const {
    if (arguments.length() == 0 || Util::String(COMMAND_SHOW).beginsWith(arguments[0])) {
        return show();
    }
    if (Util::String(COMMAND_REMOVE).beginsWith(arguments[0])) {
        return remove();
    }
    if (Util::String(COMMAND_ADD).beginsWith(arguments[0])) {
        return add();
    }

    Util::System::error << "ip: Invalid argument '" << arguments[0] << "'!" << Util::Io::PrintStream::lnFlush;
    return -1;
}

int32_t Address::show() const {
    if (arguments.length() > 1) {
        printDeviceInfo(arguments[1]);
    } else {
        for (const auto &file: Util::Io::File("/device").getChildren()) {
            const auto deviceName = file.getName();
            if (deviceName.beginsWith("eth") || deviceName.beginsWith("loopback")) {
                printDeviceInfo(deviceName);
            }
        }
    }

    return 0;
}

int32_t Address::remove() const {
    if (arguments.length() < 3) {
        Util::System::error << "ip: Missing arguments for 'address " << COMMAND_REMOVE << "'!" << Util::Io::PrintStream::lnFlush;
        return - 1;
    }

    const Util::Network::Ip4::Ip4SubnetAddress ipAddress(arguments[1]);
    const Util::String deviceName(arguments[2]);

    const Util::Io::File macFile("/device/" + deviceName + "/mac");
    if (!macFile.exists()) {
        Util::System::error << "ip: Device '" << deviceName << "' not found!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    Util::Io::FileInputStream macStream(macFile);
    const Util::Network::MacAddress macAddress(macStream.readLine().content);

    const Util::Network::Socket socket(Util::Network::Socket::ETHERNET);
    if (!socket.bind(macAddress)) {
        Util::System::error << "ip: Unable to bind ethernet socket to device '" << deviceName << "'!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    if (!socket.removeIp4Address(ipAddress)) {
        Util::System::error << "ip: Failed to remove IPv4 address '" << ipAddress.toString() << "' from device '" << deviceName << "'!" << Util::Io::PrintStream::lnFlush;
    }

    return 0;
}

int32_t Address::add() const {
    if (arguments.length() < 3) {
        Util::System::error << "ip: Missing arguments for 'address " << COMMAND_ADD << "'!" << Util::Io::PrintStream::lnFlush;
        return - 1;
    }

    const Util::Network::Ip4::Ip4SubnetAddress ipAddress(arguments[1]);
    const Util::String deviceName(arguments[2]);

    const Util::Io::File macFile("/device/" + deviceName + "/mac");
    if (!macFile.exists()) {
        Util::System::error << "ip: Device '" << deviceName << "' not found!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    Util::Io::FileInputStream macStream(macFile);
    const Util::Network::MacAddress macAddress(macStream.readLine().content);

    const Util::Network::Socket socket(Util::Network::Socket::ETHERNET);
    if (!socket.bind(macAddress)) {
        Util::System::error << "ip: Unable to bind ethernet socket to device '" << deviceName << "'!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    if (!socket.addIp4Address(ipAddress)) {
        Util::System::error << "ip: Failed to remove IPv4 address '" << ipAddress.toString() << "' from device '" << deviceName << "'!" << Util::Io::PrintStream::lnFlush;
    }

    return 0;
}

void Address::printDeviceInfo(const Util::String &deviceName) {
    const Util::Io::File macFile("/device/" + deviceName + "/mac");
    if (!macFile.exists()) {
        Util::System::error << "ip: Device '" << deviceName << "' not found!" << Util::Io::PrintStream::lnFlush;
        return;
    }

    Util::Io::FileInputStream macStream(macFile);
    const Util::Network::MacAddress macAddress(macStream.readLine().content);

    const Util::Network::Socket ethernetSocket(Util::Network::Socket::ETHERNET);
    if (!ethernetSocket.bind(macAddress)) {
        Util::System::error << "ip: Unable to bind ethernet socket to device '" << deviceName << "'!" << Util::Io::PrintStream::lnFlush;
        return;
    }

    const auto ipAddresses = ethernetSocket.getIp4Addresses();

    Util::System::out << deviceName << ":" << Util::Io::PrintStream::ln
                      << "    MAC: " << macAddress.toString() << Util::Io::PrintStream::ln;

    for (const auto &address : ipAddresses) {
        Util::System::out << "    IPv4: " << address.toString() << Util::Io::PrintStream::ln;
    }

    Util::System::out << Util::Io::PrintStream::flush;
}


