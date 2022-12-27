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
 *
 * The network stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "NetworkService.h"

#include <cstdarg>

#include "device/network/NetworkFilesystemDriver.h"
#include "device/network/NetworkDevice.h"
#include "lib/util/Exception.h"
#include "lib/util/network/MacAddress.h"
#include "lib/util/network/NetworkAddress.h"
#include "lib/util/network/Socket.h"
#include "kernel/network/ip4/Ip4Module.h"
#include "kernel/network/ip4/Ip4RoutingModule.h"
#include "kernel/system/SystemCall.h"
#include "kernel/system/System.h"
#include "kernel/network/ethernet/EthernetSocket.h"
#include "kernel/network/ip4/Ip4Socket.h"
#include "kernel/network/icmp/IcmpSocket.h"
#include "kernel/network/udp/UdpSocket.h"
#include "lib/util/system/System.h"
#include "FilesystemService.h"
#include "MemoryService.h"
#include "lib/util/memory/Address.h"
#include "lib/util/network/Datagram.h"
#include "kernel/network/Socket.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Kernel {
namespace Network {
namespace Ip4 {
class Ip4Route;
}  // namespace Ip4
}  // namespace Network

NetworkService::NetworkService() {
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

    SystemCall::registerSystemCall(Util::System::SEND_DATAGRAM, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 2) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto fileDescriptor = va_arg(arguments, int32_t);
        auto *datagram = va_arg(arguments, Util::Network::Datagram*);

        auto &filesystemService = System::getService<FilesystemService>();
        auto &socket = reinterpret_cast<Network::Socket &>(filesystemService.getNode(fileDescriptor));

        return socket.send(*datagram) ? Util::System::OK : Util::System::ILLEGAL_STATE;
    });

    SystemCall::registerSystemCall(Util::System::RECEIVE_DATAGRAM, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 2) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto fileDescriptor = va_arg(arguments, int32_t);
        auto *datagram = va_arg(arguments, Util::Network::Datagram*);

        auto &filesystemService = System::getService<FilesystemService>();
        auto &memoryService = System::getService<MemoryService>();
        auto &socket = reinterpret_cast<Network::Socket &>(filesystemService.getNode(fileDescriptor));

        auto *kernelDatagram = socket.receive();
        auto *datagramBuffer = reinterpret_cast<uint8_t *>(memoryService.allocateUserMemory(kernelDatagram->getLength()));

        auto source = Util::Memory::Address<uint32_t>(kernelDatagram->getData());
        auto target = Util::Memory::Address<uint32_t>(datagramBuffer);
        target.copyRange(source, kernelDatagram->getLength());

        datagram->setData(datagramBuffer, kernelDatagram->getLength());
        datagram->setRemoteAddress(kernelDatagram->getRemoteAddress());
        datagram->setAttributes(*kernelDatagram);

        delete kernelDatagram;
        return Util::System::OK;
    });
}

void NetworkService::registerNetworkDevice(Device::Network::NetworkDevice *device) {
    devices.add(device);
    Device::Network::NetworkFilesystemDriver::mount(*device);
}

Device::Network::NetworkDevice &NetworkService::getNetworkDevice(const Util::Memory::String &identifier) {
    for (auto *device: devices) {
        if (device->getIdentifier() == identifier) {
            return *device;
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "NetworkService: Device not found!");
}

Device::Network::NetworkDevice &NetworkService::getNetworkDevice(const Util::Network::MacAddress &address) {
    for (auto *device: devices) {
        if (device->getMacAddress() == address) {
            return *device;
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "NetworkService: Device not found!");
}

Network::NetworkStack &NetworkService::getNetworkStack() {
    return networkStack;
}

void NetworkService::registerIp4Route(const Network::Ip4::Ip4Route &route) {
    networkStack.getIp4Module().getRoutingModule().addRoute(route);
}

void NetworkService::setDefaultRoute(const Network::Ip4::Ip4Route &route) {
    networkStack.getIp4Module().getRoutingModule().setDefaultRoute(route);
}

int32_t NetworkService::createSocket(Util::Network::Socket::Type socketType) {
    Filesystem::Node *socket;
    switch (socketType) {
        case Util::Network::Socket::ETHERNET:
            socket = reinterpret_cast<Filesystem::Node *>(new Network::Ethernet::EthernetSocket());
            break;
        case Util::Network::Socket::IP4:
            socket = reinterpret_cast<Filesystem::Node *>(new Network::Ip4::Ip4Socket());
            break;
        case Util::Network::Socket::ICMP:
            socket = reinterpret_cast<Filesystem::Node *>(new Network::Icmp::IcmpSocket());
            break;
        case Util::Network::Socket::UDP:
            socket = reinterpret_cast<Filesystem::Node *>(new Network::Udp::UdpSocket());
            break;
        default:
            return Util::System::INVALID_ARGUMENT;
    }

    auto &filesystemService = System::getService<FilesystemService>();
    return filesystemService.registerFile(socket);
}

}