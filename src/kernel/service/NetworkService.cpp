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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "NetworkService.h"

#include <stdarg.h>

#include "device/network/NetworkDevice.h"
#include "lib/util/base/Panic.h"
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
#include "util/network/ethernet/EthernetDatagram.h"
#include "util/network/icmp/IcmpDatagram.h"
#include "util/network/ip4/Ip4Datagram.h"
#include "util/network/udp/UdpDatagram.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Kernel {

Util::HashMap<Util::String, uint32_t> NetworkService::nameMap;

NetworkService::NetworkService() {
    ASSIGN_SYSTEM_CALL(Util::System::CREATE_SOCKET, NetworkService::systemCallCreateSocket);
    ASSIGN_SYSTEM_CALL(Util::System::SEND_DATAGRAM, NetworkService::systemCallSendDatagram);
    ASSIGN_SYSTEM_CALL(Util::System::RECEIVE_DATAGRAM, NetworkService::systemCallReceiveDatagram);
}

void NetworkService::initializeLoopback() {
    auto *loopback = new Device::Network::Loopback();
    loopback->setIdentifier("loopback");

    lock.acquire();
    deviceMap.put(loopback->getIdentifier(), loopback);
    LOG_INFO("Registered device [%s]",static_cast<const char*>(loopback->getIdentifier()));
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
    device->setIdentifier(Util::String::format("%s%u", static_cast<const char*>(deviceClass), value));
    deviceMap.put(device->getIdentifier(), device);
    nameMap.put(deviceClass, value + 1);

    LOG_INFO("Registered device [%s]",static_cast<const char*>(device->getIdentifier()));
    lock.release();

    Device::Network::NetworkFilesystemDriver::mount(*device);
    return device->getIdentifier();
}

Device::Network::NetworkDevice &NetworkService::getNetworkDevice(const Util::String &identifier) {
    lock.acquire();
    if (!deviceMap.containsKey(identifier)) {
        lock.release();
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "NetworkService: Device not found!");
    }

    auto &result = *deviceMap.get(identifier);
    lock.release();

    return result;
}

Device::Network::NetworkDevice &NetworkService::getNetworkDevice(const Util::Network::MacAddress &address) {
    for (auto *device: deviceMap.getValues()) {
        if (device->getMacAddress() == address) {
            return *device;
        }
    }

    Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "NetworkService: Device not found!");
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

int64_t NetworkService::systemCallCreateSocket(const Util::Network::Socket::Type socketType) {
    auto &networkService = getService<NetworkService>();

    return networkService.createSocket(socketType);
}

int64_t NetworkService::systemCallSendDatagram(const int32_t fileDescriptor, const uint8_t *remoteAddressBuffer,
    const uint8_t *payload, const uint32_t length, const uint32_t datagramArg)
{
    auto &filesystemService = getService<FilesystemService>();

    const auto &descriptor = filesystemService.getFileDescriptor(fileDescriptor);
    if (!descriptor.isValid()) {
        return -1;
    }

    auto &socket = reinterpret_cast<Network::Socket&>(descriptor.getNode());
    if (!socket.isBound()) {
        return -1;
    }

    switch (socket.getNetworkType()) {
        case Util::Network::Socket::ETHERNET: {
            const auto remoteAddress = Util::Network::MacAddress(remoteAddressBuffer);
            const auto etherType = static_cast<Util::Network::Ethernet::EthernetHeader::EtherType>(datagramArg);
            const auto datagram = Util::Network::Ethernet::EthernetDatagram(payload, length, remoteAddress, etherType);

            return socket.send(datagram) ? 0 : -1;
        }
        case Util::Network::Socket::IP4: {
            const auto remoteAddress = Util::Network::Ip4::Ip4Address(remoteAddressBuffer);
            const auto protocol = static_cast<Util::Network::Ip4::Ip4Header::Protocol>(datagramArg);
            const auto datagram = Util::Network::Ip4::Ip4Datagram(payload, length, remoteAddress, protocol);

            return socket.send(datagram) ? 0 : -1;
        }
        case Util::Network::Socket::ICMP: {
            const auto remoteAddress = Util::Network::Ip4::Ip4Address(remoteAddressBuffer);
            const auto type = static_cast<Util::Network::Icmp::IcmpHeader::Type>(datagramArg & 0xff);
            const auto code = static_cast<uint8_t>(datagramArg >> 8);
            const auto datagram = Util::Network::Icmp::IcmpDatagram(payload, length, remoteAddress, type, code);

            return socket.send(datagram) ? 0 : -1;
        }
        case Util::Network::Socket::UDP: {
            const auto remoteAddress = Util::Network::Ip4::Ip4PortAddress(remoteAddressBuffer);
            const auto datagram = Util::Network::Udp::UdpDatagram(payload, length, remoteAddress);

            return socket.send(datagram) ? 0 : -1;
        }
        case Util::Network::Socket::IP6:
        case Util::Network::Socket::TCP:
        default:
            return -1;
    }
}

int64_t NetworkService::systemCallReceiveDatagram(const int32_t fileDescriptor, uint8_t **remoteAddressBuffer,
    uint8_t **payload, uint32_t *length, uint32_t *datagramArg)
{
    auto &filesystemService = getService<FilesystemService>();
    auto &memoryService = getService<MemoryService>();

    const auto &descriptor = filesystemService.getFileDescriptor(fileDescriptor);
    if (!descriptor.isValid()) {
        return -1;
    }

    auto &socket = reinterpret_cast<Network::Socket&>(descriptor.getNode());
    if (!socket.isBound()) {
        return -1;
    }

    if (descriptor.getAccessMode() == Util::Io::File::BLOCKING || socket.isReadyToRead()) {
        // Receive the datagram from the socket (will block if no datagram is available)
        // If the descriptor is non-blocking, we have already checked if the socket is ready to read
        auto *kernelDatagram = socket.receive();
        if (kernelDatagram == nullptr) {
            return -1;
        }

        switch (socket.getNetworkType()) {
            case Util::Network::Socket::ETHERNET: {
                const auto *ethernetDatagram = reinterpret_cast<Util::Network::Ethernet::EthernetDatagram*>(kernelDatagram);
                *datagramArg = ethernetDatagram->getEtherType();
            }
            break;
            case Util::Network::Socket::IP4: {
                const auto *ip4Datagram = reinterpret_cast<Util::Network::Ip4::Ip4Datagram*>(kernelDatagram);
                *datagramArg = ip4Datagram->getProtocol();
            }
            break;
            case Util::Network::Socket::ICMP: {
                const auto *icmpDatagram = reinterpret_cast<Util::Network::Icmp::IcmpDatagram*>(kernelDatagram);
                *datagramArg = static_cast<uint32_t>(icmpDatagram->getCode() << 8) | static_cast<uint32_t>(icmpDatagram->getType());
            }
            break;
            case Util::Network::Socket::UDP:
                break;
            case Util::Network::Socket::IP6:
            case Util::Network::Socket::TCP:
                delete kernelDatagram;
                return -1;
        }

        const auto &remoteAddress = kernelDatagram->getRemoteAddress();
        *remoteAddressBuffer = static_cast<uint8_t*>(memoryService.allocateUserMemory(remoteAddress.getLength()));
        remoteAddress.getAddress(*remoteAddressBuffer);

        *length = kernelDatagram->getLength();
        *payload = static_cast<uint8_t*>(memoryService.allocateUserMemory(*length));
        const auto payloadSource = Util::Address(kernelDatagram->getData());
        const auto payloadTarget = Util::Address(*payload);
        payloadTarget.copyRange(payloadSource, *length);

        delete kernelDatagram;
        return 0;
    }

    // The descriptor is non-blocking and the socket is not ready to read
    return -1;
}

}