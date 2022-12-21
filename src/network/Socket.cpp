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

#include "Socket.h"

#include "lib/util/Exception.h"
#include "lib/util/network/NetworkAddress.h"
#include "lib/util/network/Socket.h"

namespace Network {

void Socket::bind(const Util::Network::NetworkAddress &address) {
    if (bindAddress != nullptr) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Socket: Already bound!");
    }

    bindAddress = address.createCopy();
    performBind();
}

Socket::~Socket() {
    delete bindAddress;
}

const Util::Network::NetworkAddress& Socket::getAddress() const {
    return *bindAddress;
}

bool Socket::control(uint32_t request, const Util::Data::Array<uint32_t> &parameters) {
    switch (request) {
        case Util::Network::Socket::Request::BIND: {
            bind(*reinterpret_cast<Util::Network::NetworkAddress *>(parameters[0]));
            return true;
        }
        case Util::Network::Socket::Request::GET_LOCAL_ADDRESS: {
            auto **address = reinterpret_cast<Util::Network::NetworkAddress**>(parameters[0]);
            *address = bindAddress->createCopy();
            return true;
        }
        default:
            return false;
    }
}

}
