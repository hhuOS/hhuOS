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

#include "Socket.h"

#include "lib/util/base/Panic.h"
#include "lib/util/network/NetworkAddress.h"
#include "lib/util/network/Socket.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"
#include "lib/util/network/MacAddress.h"
#include "lib/util/network/ip4/Ip4PortAddress.h"
#include "kernel/network/NetworkModule.h"
#include "kernel/service/NetworkService.h"
#include "device/network/NetworkDevice.h"
#include "kernel/network/NetworkStack.h"
#include "kernel/network/ip4/Ip4Interface.h"
#include "kernel/network/ip4/Ip4Module.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/base/Address.h"
#include "lib/util/network/ip4/Ip4SubnetAddress.h"
#include "kernel/network/ip4/Ip4RoutingModule.h"
#include "lib/util/base/String.h"
#include "lib/util/network/ip4/Ip4Route.h"
#include "kernel/service/Service.h"
#include "lib/util/collection/Array.h"
#include "util/network/udp/UdpDatagram.h"

namespace Kernel::Network {

Socket::Socket(NetworkModule &networkModule, Util::Network::Socket::Type type) : networkModule(networkModule), type(type) {}

void Socket::bind(const Util::Network::NetworkAddress &address) {
    if (bindAddress != nullptr) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Socket: Already bound!");
    }

    /*
     * We cannot use createCopy() here, because the given address may be a user space object.
     * Since it is a reference of an abstract type, the object's vtable is used to perform the call to createCopy().
     * This will result in a call to the user space variant of this function, which will allocate memory on the user space heap.
     */

    auto addressStream = Util::Io::ByteArrayOutputStream();
    address.write(addressStream);

    switch (address.getType()) {
        case Util::Network::NetworkAddress::MAC:
            bindAddress = new Util::Network::MacAddress(addressStream.getBuffer());
            break;
        case Util::Network::NetworkAddress::IP4:
            bindAddress = new Util::Network::Ip4::Ip4Address(addressStream.getBuffer());
            break;
        case Util::Network::NetworkAddress::IP4_PORT:
            bindAddress = new Util::Network::Ip4::Ip4PortAddress(addressStream.getBuffer());
            break;
        default:
            Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Socket: Illegal address type for bind()!");
    }

    if (!networkModule.registerSocket(*this)) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Failed to register socket!");
    }
}

Socket::~Socket() {
    networkModule.deregisterSocket(*this);
    delete bindAddress;
}

const Util::Network::NetworkAddress& Socket::getAddress() const {
    if (!isBound()) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Socket: Not bound!");
    }

    return *bindAddress;
}

bool Socket::isBound() const {
    return bindAddress != nullptr;
}

void Socket::setTimeout(uint32_t timeout) {
    Socket::timeout = timeout;
}

Util::Network::Socket::Type Socket::getNetworkType() const {
    return type;
}

int64_t Socket::control(uint32_t request, uint32_t arg0, uint32_t arg1, uint32_t) {
    switch (request) {
        case Util::Network::Socket::Request::SET_TIMEOUT: {
            timeout = arg0;
            return 0;
        }
        case Util::Network::Socket::Request::BIND: {
            const auto *addressBuffer = reinterpret_cast<const uint8_t*>(arg0);

            switch (type) {
                case Util::Network::Socket::ETHERNET:
                    bind(Util::Network::MacAddress(addressBuffer));
                    return 0;
                case Util::Network::Socket::IP4:
                case Util::Network::Socket::ICMP:
                    bind(Util::Network::Ip4::Ip4Address(addressBuffer));
                    return 0;
                case Util::Network::Socket::UDP:
                case Util::Network::Socket::TCP:
                    bind(Util::Network::Ip4::Ip4PortAddress(addressBuffer));
                    return 0;
                case Util::Network::Socket::IP6:
                    return -1;
            }
            break;
        }
        case Util::Network::Socket::Request::GET_LOCAL_ADDRESS: {
            if (!isBound()) {
                return -1;
            }

            auto *buffer = reinterpret_cast<uint8_t*>(arg0);
            bindAddress->getAddress(buffer);
            return 0;
        }
        case Util::Network::Socket::Request::GET_IP4_ADDRESSES: {
            if (type != Util::Network::Socket::ETHERNET) {
                return -1;
            }
            if (!isBound()) {
                return -1;
            }

            auto &memoryService = Service::getService<MemoryService>();
            auto &networkService = Service::getService<NetworkService>();
            auto &ip4Module = networkService.getNetworkStack().getIp4Module();
            const auto &device = networkService.getNetworkDevice(
                reinterpret_cast<Util::Network::MacAddress&>(*bindAddress));

            auto interfaces = ip4Module.getInterfaces(device.getIdentifier());
            const auto count = interfaces.length();
            auto *buffer = static_cast<uint8_t*>(
                memoryService.allocateUserMemory(count * Util::Network::Ip4::Ip4SubnetAddress::ADDRESS_LENGTH));

            for (uint32_t i = 0; i < count; i++) {
                interfaces[i].getSubnetAddress().getAddress(buffer + i * Util::Network::Ip4::Ip4SubnetAddress::ADDRESS_LENGTH);
            }

            *reinterpret_cast<uint8_t**>(arg0) = buffer;
            return count;
        }
        case Util::Network::Socket::Request::REMOVE_IP4_ADDRESS: {
            if (type != Util::Network::Socket::ETHERNET) {
                return -1;
            }
            if (!isBound()) {
                return -1;
            }

            auto &networkService = Service::getService<NetworkService>();
            auto &ip4Module = networkService.getNetworkStack().getIp4Module();
            const auto &device = networkService.getNetworkDevice(reinterpret_cast<Util::Network::MacAddress&>(*bindAddress));
            auto *buffer = reinterpret_cast<uint8_t*>(arg0);
            const auto address = Util::Network::Ip4::Ip4SubnetAddress(buffer);

            return ip4Module.removeInterface(address, device.getIdentifier()) ? 0 : -1;
        }
        case Util::Network::Socket::Request::ADD_IP4_ADDRESS: {
            if (type != Util::Network::Socket::ETHERNET) {
                return -1;
            }
            if (!isBound()) {
                return -1;
            }

            auto &networkService = Service::getService<NetworkService>();
            auto &ip4Module = networkService.getNetworkStack().getIp4Module();
            auto &device = networkService.getNetworkDevice(reinterpret_cast<Util::Network::MacAddress&>(*bindAddress));
            auto *buffer = reinterpret_cast<uint8_t*>(arg0);
            const auto address = Util::Network::Ip4::Ip4SubnetAddress(buffer);

            return ip4Module.registerInterface(address, device) ? 0 : -1;
        }
        case Util::Network::Socket::Request::GET_ROUTES: {
            if (type != Util::Network::Socket::IP4) {
                return -1;
            }
            if (!isBound()) {
                return -1;
            }

            auto &memoryService = Service::getService<MemoryService>();
            auto &networkService = Service::getService<NetworkService>();
            auto &routingModule = networkService.getNetworkStack().getIp4Module().getRoutingModule();

            const auto routes = routingModule.getRoutes(*reinterpret_cast<Util::Network::Ip4::Ip4Address*>(bindAddress));
            const auto count = routes.length();

            constexpr auto ENTRY_LENGTH = 2 * Util::Network::Ip4::Ip4SubnetAddress::ADDRESS_LENGTH + Util::Network::Ip4::Ip4SubnetAddress::ADDRESS_LENGTH;
            auto **devices = static_cast<char**>(memoryService.allocateUserMemory(count * sizeof(char*)));
            auto *addressBuffer = static_cast<uint8_t*>(memoryService.allocateUserMemory(count * ENTRY_LENGTH));

            for (uint32_t i = 0; i < count; i++) {
                const auto &route = routes[i];

                devices[i] = static_cast<char*>(memoryService.allocateUserMemory((route.getDeviceIdentifier().length() + 1) * sizeof(char)));
                const auto source = Util::Address(static_cast<const char*>(route.getDeviceIdentifier()));
                const auto target = Util::Address(devices[i]);
                target.copyString(source);

                route.getSourceAddress().getAddress(addressBuffer + i * ENTRY_LENGTH);
                route.getTargetAddress().getAddress(addressBuffer + i * ENTRY_LENGTH + Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH);

                if (route.hasNextHop()) {
                    route.getNextHop().getAddress(addressBuffer + i * ENTRY_LENGTH + Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH + Util::Network::Ip4::Ip4SubnetAddress::ADDRESS_LENGTH);
                } else {
                    const auto nextHopAddress = Util::Address(addressBuffer).add(i * ENTRY_LENGTH + Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH + Util::Network::Ip4::Ip4SubnetAddress::ADDRESS_LENGTH);
                    nextHopAddress.setRange(0, Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH);
                }
            }

            *reinterpret_cast<uint8_t**>(arg0) = addressBuffer;
            *reinterpret_cast<char***>(arg1) = devices;
            return count;
        }
        case Util::Network::Socket::Request::REMOVE_ROUTE: {
            if (type != Util::Network::Socket::IP4) {
                return -1;
            }
            if (!isBound()) {
                return -1;
            }

            auto &networkService = Service::getService<NetworkService>();
            auto &routingModule = networkService.getNetworkStack().getIp4Module().getRoutingModule();

            auto *buffer = reinterpret_cast<uint8_t*>(arg0);
            auto *device = reinterpret_cast<char*>(arg1);

            const auto sourceAddress = Util::Network::Ip4::Ip4Address(buffer);
            const auto targetAddress = Util::Network::Ip4::Ip4SubnetAddress(buffer + Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH);
            const auto nextHop = Util::Network::Ip4::Ip4Address(buffer + Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH + Util::Network::Ip4::Ip4SubnetAddress::ADDRESS_LENGTH);

            if (nextHop == Util::Network::Ip4::Ip4Address::ANY) {
                const auto route = Util::Network::Ip4::Ip4Route(sourceAddress, targetAddress, device);
                return routingModule.removeRoute(route) ? 0 : -1;
            }

            const auto route = Util::Network::Ip4::Ip4Route(sourceAddress, targetAddress, nextHop, device);
            return routingModule.removeRoute(route) ? 0 : -1;
        }
        case Util::Network::Socket::Request::ADD_ROUTE: {
            if (type != Util::Network::Socket::IP4) {
                return -1;
            }
            if (!isBound()) {
                return -1;
            }

            auto &networkService = Service::getService<NetworkService>();
            auto &routingModule = networkService.getNetworkStack().getIp4Module().getRoutingModule();

            auto *buffer = reinterpret_cast<uint8_t*>(arg0);
            auto *device = reinterpret_cast<char*>(arg1);

            const auto sourceAddress = Util::Network::Ip4::Ip4Address(buffer);
            const auto targetAddress = Util::Network::Ip4::Ip4SubnetAddress(buffer + Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH);
            const auto nextHop = Util::Network::Ip4::Ip4Address(buffer + Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH + Util::Network::Ip4::Ip4SubnetAddress::ADDRESS_LENGTH);

            if (nextHop == Util::Network::Ip4::Ip4Address::ANY) {
                const auto route = Util::Network::Ip4::Ip4Route(sourceAddress, targetAddress, device);
                return routingModule.addRoute(route) ? 0 : -1;
            }

            const auto route = Util::Network::Ip4::Ip4Route(sourceAddress, targetAddress, nextHop, device);
            return routingModule.addRoute(route) ? 0 : -1;
        }
        default:
            return -1;
    }

    return -1;
}

}
