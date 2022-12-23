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
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "Socket.h"

#include "lib/util/Exception.h"
#include "lib/util/network/NetworkAddress.h"
#include "lib/util/network/Socket.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "lib/util/network/MacAddress.h"
#include "lib/util/network/ip4/Ip4PortAddress.h"
#include "kernel/network/NetworkModule.h"

namespace Kernel::Network {

Socket::Socket(NetworkModule &networkModule) : networkModule(networkModule) {}

void Socket::bind(const Util::Network::NetworkAddress &address) {
    if (bindAddress != nullptr) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Already bound!");
    }

    /*
     * We cannot use createCopy() here, because the given address may be a user space object.
     * Since it is a reference of an abstract type, the object's vtable is used to perform the call to createCopy().
     * This will result in a call the user space variant of this function, which will allocate memory on the user space heap.
     */

    auto addressStream = Util::Stream::ByteArrayOutputStream();
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
    return *bindAddress;
}

bool Socket::control(uint32_t request, const Util::Data::Array<uint32_t> &parameters) {
    switch (request) {
        case Util::Network::Socket::Request::BIND: {
            bind(*reinterpret_cast<Util::Network::NetworkAddress*>(parameters[0]));
            return true;
        }
        case Util::Network::Socket::Request::GET_LOCAL_ADDRESS: {
            if (bindAddress == nullptr) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Not yet bound!");
            }

            auto *address = reinterpret_cast<Util::Network::NetworkAddress*>(parameters[0]);
            address->setAddress(*bindAddress);
            return true;
        }
        default:
            return false;
    }
}

}
