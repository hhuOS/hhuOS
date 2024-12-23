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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "NetworkService.h"

#include <stdarg.h>

#include "device/network/NetworkDevice.h"
#include "lib/util/base/Exception.h"
#include "lib/util/network/MacAddress.h"
#include "lib/util/network/NetworkAddress.h"
#include "lib/util/network/Socket.h"
#include "kernel/network/ip4/Ip4Module.h"
#include "kernel/network/ethernet/EthernetSocket.h"
#include "kernel/network/ip4/Ip4Socket.h"
#include "kernel/network/icmp/IcmpSocket.h"
#include "kernel/network/udp/UdpSocket.h"
#include "lib/util/base/System.h"
#include "FilesystemService.h"
#include "MemoryService.h"
#include "lib/util/base/Address.h"
#include "lib/util/network/Datagram.h"
#include "kernel/network/Socket.h"
#include "kernel/log/Log.h"
#include "device/network/NetworkFilesystemDriver.h"
#include "device/network/loopback/Loopback.h"
#include "kernel/network/arp/ArpModule.h"
#include "kernel/network/ip4/Ip4RoutingModule.h"
#include "lib/util/network/ip4/Ip4Route.h"
#include "lib/util/network/ip4/Ip4SubnetAddress.h"
#include "lib/util/collection/Array.h"
#include "InterruptService.h"
#include "kernel/service/Service.h"
#include "kernel/process/FileDescriptor.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Kernel {

Util::HashMap<Util::String, uint32_t> NetworkService::nameMap;

NetworkService::NetworkService() {
    Service::getService<InterruptService>().assignSystemCall(Util::System::CREATE_SOCKET, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 2) {
            return false;
        }

        auto &networkService = Service::getService<NetworkService>();
        auto socketType = static_cast<Util::Network::Socket::Type>(va_arg(arguments, int));
        auto &fileDescriptor = *va_arg(arguments, int32_t*);

        fileDescriptor = networkService.createSocket(socketType);
        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::SEND_DATAGRAM, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 2) {
            return false;
        }

        auto &filesystemService = Service::getService<FilesystemService>();
        auto fileDescriptor = va_arg(arguments, int32_t);
        auto &datagram = *va_arg(arguments, Util::Network::Datagram*);

        auto &socket = reinterpret_cast<Network::Socket&>(filesystemService.getFileDescriptor(fileDescriptor).getNode());
        if (!socket.isBound()) {
            Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not yet bound!");
        }

        return socket.send(datagram);
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::RECEIVE_DATAGRAM, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 2) {
            return false;
        }

        auto &filesystemService = Service::getService<FilesystemService>();
        auto &memoryService = Service::getService<MemoryService>();
        auto fileDescriptor = va_arg(arguments, int32_t);
        auto &datagram = *va_arg(arguments, Util::Network::Datagram*);

        auto &socketDescriptor = filesystemService.getFileDescriptor(fileDescriptor);
        auto &socket = reinterpret_cast<Network::Socket&>(socketDescriptor.getNode());
        if (!socket.isBound()) {
            Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not yet bound!");
        }

        if (socketDescriptor.getAccessMode() == Util::Io::File::BLOCKING || socket.isReadyToRead()) {
            // Receive the datagram from the socket (will block if no datagram is available)
            // If the descriptor is non-blocking, we have already checked if the socket is ready to read
            auto *kernelDatagram = socket.receive();
            if (kernelDatagram == nullptr) {
                return false;
            }

            auto *datagramBuffer = reinterpret_cast<uint8_t *>(memoryService.allocateUserMemory(kernelDatagram->getLength()));

            auto source = Util::Address<uint32_t>(kernelDatagram->getData());
            auto target = Util::Address<uint32_t>(datagramBuffer);
            target.copyRange(source, kernelDatagram->getLength());

            datagram.setData(datagramBuffer, kernelDatagram->getLength());
            datagram.setRemoteAddress(kernelDatagram->getRemoteAddress());
            datagram.setAttributes(*kernelDatagram);

            delete kernelDatagram;
            return true;
        } else {
            // The descriptor is non-blocking and the socket is not ready to read
            return false;
        }
    });
}

void NetworkService::initializeLoopback() {
    auto *loopback = new Device::Network::Loopback();
    loopback->setIdentifier("loopback");

    lock.acquire();
    deviceMap.put(loopback->getIdentifier(), loopback);
    LOG_INFO("Registered device [%s]",static_cast<char*>(loopback->getIdentifier()));
    lock.release();

    Device::Network::NetworkFilesystemDriver::mount(*loopback);

    auto address = Util::Network::Ip4::Ip4SubnetAddress("127.0.0.1/8");
    networkStack.getIp4Module().registerInterface(address, *loopback);
    networkStack.getIp4Module().getRoutingModule().addRoute(Util::Network::Ip4::Ip4Route(address, loopback->getIdentifier()));
    networkStack.getArpModule().setEntry(address.getIp4Address(), loopback->getMacAddress());
}

Util::String NetworkService::registerNetworkDevice(Device::Network::NetworkDevice *device, const Util::String &deviceClass) {
    lock.acquire();
    if (!nameMap.containsKey(deviceClass)) {
        nameMap.put(deviceClass, 0);
    }

    auto value = nameMap.get(deviceClass);
    device->setIdentifier(Util::String::format("%s%u", static_cast<char*>(deviceClass), value));
    deviceMap.put(device->getIdentifier(), device);
    nameMap.put(deviceClass, value + 1);

    LOG_INFO("Registered device [%s]",static_cast<char*>(device->getIdentifier()));
    lock.release();

    Device::Network::NetworkFilesystemDriver::mount(*device);
    return device->getIdentifier();
}

Device::Network::NetworkDevice &NetworkService::getNetworkDevice(const Util::String &identifier) {
    lock.acquire();
    if (!deviceMap.containsKey(identifier)) {
        lock.release();
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "NetworkService: Device not found!");
    }

    auto &result = *deviceMap.get(identifier);
    lock.release();

    return result;
}

Device::Network::NetworkDevice &NetworkService::getNetworkDevice(const Util::Network::MacAddress &address) {
    for (auto *device: deviceMap.values()) {
        if (device->getMacAddress() == address) {
            return *device;
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "NetworkService: Device not found!");
}

Network::NetworkStack &NetworkService::getNetworkStack() {
    return networkStack;
}

int32_t NetworkService::createSocket(Util::Network::Socket::Type socketType) {
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
            return false;
    }

    auto &filesystemService = Service::getService<FilesystemService>();
    return filesystemService.registerFile(socket);
}

bool NetworkService::isNetworkDeviceRegistered(const Util::String &identifier) {
    return deviceMap.containsKey(identifier);
}

}