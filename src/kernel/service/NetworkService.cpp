/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#include "NetworkService.h"

#include <stdarg.h>

#include "device/network/NetworkFilesystemDriver.h"
#include "device/network/NetworkDevice.h"
#include "lib/util/Exception.h"
#include "lib/util/network/MacAddress.h"
#include "lib/util/network/NetworkAddress.h"
#include "lib/util/network/Socket.h"
#include "network/ip4/Ip4Module.h"
#include "network/ip4/Ip4RoutingModule.h"
#include "kernel/system/SystemCall.h"
#include "ProcessService.h"
#include "kernel/system/System.h"
#include "kernel/process/Process.h"
#include "network/ethernet/EthernetSocket.h"
#include "network/ip4/Ip4Socket.h"
#include "network/icmp/IcmpSocket.h"
#include "network/udp/UdpSocket.h"
#include "kernel/file/FileDescriptorManager.h"
#include "lib/util/system/System.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Network {
namespace Ip4 {
class Ip4Route;
}  // namespace Ip4
}  // namespace Network

Kernel::NetworkService::NetworkService() {
    SystemCall::registerSystemCall(Util::System::CREATE_SOCKET, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 2) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto socketType = static_cast<Util::Network::Socket::Type>(va_arg(arguments, int));
        auto *fileDescriptor = va_arg(arguments, int32_t*);

        auto &networkService = System::getService<NetworkService>();
        *fileDescriptor = networkService.createSocket(socketType);

        return Util::System::OK;
    });
}

void Kernel::NetworkService::registerNetworkDevice(Device::Network::NetworkDevice *device) {
    devices.add(device);
    Device::Network::NetworkFilesystemDriver::mount(*device);
}

Device::Network::NetworkDevice& Kernel::NetworkService::getNetworkDevice(const Util::Memory::String &identifier) {
    for (auto *device : devices) {
        if (device->getIdentifier() == identifier) {
            return *device;
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "NetworkService: Device not found!");
}

Device::Network::NetworkDevice &Kernel::NetworkService::getNetworkDevice(const Util::Network::MacAddress &address) {
    for (auto *device : devices) {
        if (device->getMacAddress() == address) {
            return *device;
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "NetworkService: Device not found!");
}

::Network::NetworkStack &Kernel::NetworkService::getNetworkStack() {
    return networkStack;
}

void Kernel::NetworkService::registerIp4Route(const Network::Ip4::Ip4Route &route) {
    networkStack.getIp4Module().getRoutingModule().addRoute(route);
}

void Kernel::NetworkService::setDefaultRoute(const Network::Ip4::Ip4Route &route) {
    networkStack.getIp4Module().getRoutingModule().setDefaultRoute(route);
}

int32_t Kernel::NetworkService::createSocket(Util::Network::Socket::Type socketType) {
    Filesystem::Node *socket;
    switch (socketType) {
        case Util::Network::Socket::ETHERNET:
            socket = reinterpret_cast<Filesystem::Node*>(new Network::Ethernet::EthernetSocket());
            break;
        case Util::Network::Socket::IP4:
            socket = reinterpret_cast<Filesystem::Node*>(new Network::Ip4::Ip4Socket());
            break;
        case Util::Network::Socket::ICMP:
            socket = reinterpret_cast<Filesystem::Node*>(new Network::Icmp::IcmpSocket());
            break;
        case Util::Network::Socket::UDP:
            socket = reinterpret_cast<Filesystem::Node*>(new Network::Udp::UdpSocket());
            break;
        default:
            return Util::System::INVALID_ARGUMENT;
    }

    auto &processService = System::getService<ProcessService>();
    return processService.getCurrentProcess().getFileDescriptorManager().registerFile(socket);
}
