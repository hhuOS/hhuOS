/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#include "lib/util/base/Exception.h"
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

namespace Kernel::Network {

Socket::Socket(NetworkModule &networkModule, Util::Network::Socket::Type type) : networkModule(networkModule), type(type) {}

void Socket::bind(const Util::Network::NetworkAddress &address) {
    if (bindAddress != nullptr) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Already bound!");
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
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Socket: Illegal address type for bind()!");
    }

    if (!networkModule.registerSocket(*this)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Failed to register socket!");
    }
}

Socket::~Socket() {
    networkModule.deregisterSocket(*this);
    delete bindAddress;
}

const Util::Network::NetworkAddress& Socket::getAddress() const {
    if (!isBound()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not yet bound!");
    }

    return *bindAddress;
}

bool Socket::isBound() const {
    return bindAddress != nullptr;
}

void Socket::setTimeout(uint32_t timeout) {
    Socket::timeout = timeout;
}

bool Socket::control(uint32_t request, const Util::Array<uint32_t> &parameters) {
    switch (request) {
        case Util::Network::Socket::Request::SET_TIMEOUT: {
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Socket: Missing parameters!");
            }

            timeout = parameters[0];
            return true;
        }
        case Util::Network::Socket::Request::BIND: {
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Socket: Missing parameters!");
            }

            bind(*reinterpret_cast<Util::Network::NetworkAddress*>(parameters[0]));
            return true;
        }
        case Util::Network::Socket::Request::GET_LOCAL_ADDRESS: {
            if (!isBound()) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not yet bound!");
            }
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Socket: Missing parameters!");
            }

            auto &address = *reinterpret_cast<Util::Network::NetworkAddress*>(parameters[0]);
            address = *bindAddress;
            return true;
        }
        case Util::Network::Socket::Request::GET_IP4_ADDRESSES: {
            if (type != Util::Network::Socket::ETHERNET) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not an ethernet socket!");
            }
            if (!isBound()) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not yet bound!");
            }
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Socket: No parameter given!");
            }

            auto &networkService = Service::getService<NetworkService>();
            auto &ip4Module = networkService.getNetworkStack().getIp4Module();
            auto &device = networkService.getNetworkDevice(reinterpret_cast<Util::Network::MacAddress&>(*bindAddress));

            auto interfaces = ip4Module.getInterfaces(device.getIdentifier());
            auto &addresses = *reinterpret_cast<Util::Array<Util::Network::Ip4::Ip4SubnetAddress>*>(parameters[0]);

            for (uint32_t i = 0; i < interfaces.length() && i < addresses.length(); i++) {
                addresses[i] = interfaces[i].getSubnetAddress();
            }

            return true;
        }
        case Util::Network::Socket::Request::REMOVE_IP4_ADDRESS: {
            if (type != Util::Network::Socket::ETHERNET) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not an ethernet socket!");
            }
            if (!isBound()) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not yet bound!");
            }
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Socket: No parameter given!");
            }

            auto &networkService = Service::getService<NetworkService>();
            auto &ip4Module = networkService.getNetworkStack().getIp4Module();
            auto &device = networkService.getNetworkDevice(reinterpret_cast<Util::Network::MacAddress&>(*bindAddress));
            auto &address = *reinterpret_cast<Util::Network::Ip4::Ip4SubnetAddress*>(parameters[0]);

            return ip4Module.removeInterface(address, device.getIdentifier());
        }
        case Util::Network::Socket::Request::ADD_IP4_ADDRESS: {
            if (type != Util::Network::Socket::ETHERNET) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not an ethernet socket!");
            }
            if (!isBound()) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not yet bound!");
            }
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Socket: No parameter given!");
            }

            auto &networkService = Service::getService<NetworkService>();
            auto &ip4Module = networkService.getNetworkStack().getIp4Module();
            auto &device = networkService.getNetworkDevice(reinterpret_cast<Util::Network::MacAddress&>(*bindAddress));
            auto &address = *reinterpret_cast<Util::Network::Ip4::Ip4SubnetAddress*>(parameters[0]);

            return ip4Module.registerInterface(address, device);
        }
        case Util::Network::Socket::Request::GET_ROUTES: {
            if (type != Util::Network::Socket::IP4) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not an IPv4 socket!");
            }
            if (!isBound()) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not yet bound!");
            }
            if (parameters.length() < 4) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Socket: Missing parameters!");
            }

            auto &memoryService = Service::getService<MemoryService>();
            auto &networkService = Service::getService<NetworkService>();
            auto &routingModule = networkService.getNetworkStack().getIp4Module().getRoutingModule();
            auto &sourceAddresses = *reinterpret_cast<Util::Array<Util::Network::Ip4::Ip4Address>*>(parameters[0]);
            auto &targetAddresses = *reinterpret_cast<Util::Array<Util::Network::Ip4::Ip4SubnetAddress>*>(parameters[1]);
            auto &targetNextHops = *reinterpret_cast<Util::Array<Util::Network::Ip4::Ip4Address>*>(parameters[2]);
            auto &targetDevices = *reinterpret_cast<Util::Array<char*>*>(parameters[3]);

            auto routes = routingModule.getRoutes(*reinterpret_cast<Util::Network::Ip4::Ip4Address*>(bindAddress));
            for (uint32_t i = 0; i < routes.length() && i < targetAddresses.length(); i++) {
                auto route = routes[i];

                sourceAddresses[i] = route.getSourceAddress();
                targetAddresses[i] = route.getTargetAddress();
                targetNextHops[i] = route.hasNextHop() ? route.getNextHop() : Util::Network::Ip4::Ip4Address::ANY;
                targetDevices[i] = static_cast<char*>(memoryService.allocateUserMemory((route.getDeviceIdentifier().length() + 1) * sizeof(char)));

                auto source = Util::Address(static_cast<const char*>(route.getDeviceIdentifier()));
                auto target = Util::Address(targetDevices[i]);
                target.copyString(source);
            }

            return true;
        }
        case Util::Network::Socket::Request::REMOVE_ROUTE: {
            if (type != Util::Network::Socket::IP4) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not an IPv4 socket!");
            }
            if (!isBound()) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not yet bound!");
            }
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Socket: No parameter given!");
            }

            auto &networkService = Service::getService<NetworkService>();
            auto &routingModule = networkService.getNetworkStack().getIp4Module().getRoutingModule();
            auto route = *reinterpret_cast<Util::Network::Ip4::Ip4Route*>(parameters[0]);

            return routingModule.removeRoute(route);
        }
        case Util::Network::Socket::Request::ADD_ROUTE: {
            if (type != Util::Network::Socket::IP4) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not an IPv4 socket!");
            }
            if (!isBound()) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not yet bound!");
            }
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Socket: No parameter given!");
            }

            auto &networkService = Service::getService<NetworkService>();
            auto &routingModule = networkService.getNetworkStack().getIp4Module().getRoutingModule();
            auto route = *reinterpret_cast<Util::Network::Ip4::Ip4Route*>(parameters[0]);

            return routingModule.addRoute(route);
        }
        default:
            return false;
    }
}

}
