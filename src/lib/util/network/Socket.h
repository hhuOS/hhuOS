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

#ifndef HHUOS_LIB_SOCKET_H
#define HHUOS_LIB_SOCKET_H

#include <stdint.h>
#include "lib/util/io/file/File.h"

namespace Util {
template <typename T> class Array;

namespace Network {
class NetworkAddress;
class Datagram;

namespace Ip4 {
class Ip4Route;
class Ip4SubnetAddress;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Util::Network {

class Socket {

public:

    enum Type {
        ETHERNET, IP4, IP6, ICMP, UDP, TCP
    };

    enum Request {
        SET_TIMEOUT,
        BIND, GET_LOCAL_ADDRESS,
        GET_IP4_ADDRESSES, REMOVE_IP4_ADDRESS, ADD_IP4_ADDRESS,
        GET_ROUTES, REMOVE_ROUTE, ADD_ROUTE
    };

    /**
     * Copy Constructor.
     */
    Socket(const Socket &other) = delete;

    /**
     * Assignment operator.
     */
    Socket &operator=(const Socket &other) = delete;

    /**
     * Destructor.
     */
    ~Socket();

    static Socket createSocket(Type type);

    void setTimeout(uint32_t timeout) const;

    [[nodiscard]] bool bind(const NetworkAddress &address) const;

    [[nodiscard]] bool getLocalAddress(NetworkAddress &address) const;

    [[nodiscard]] bool send(const Util::Network::Datagram &datagram) const;

    [[nodiscard]] bool receive(Util::Network::Datagram &datagram) const;

    [[nodiscard]] Array<Ip4::Ip4SubnetAddress> getIp4Addresses() const;

    [[nodiscard]] bool removeIp4Address(const Ip4::Ip4SubnetAddress &address) const;

    [[nodiscard]] bool addIp4Address(const Ip4::Ip4SubnetAddress &address) const;

    [[nodiscard]] Array<Ip4::Ip4Route> getRoutes() const;

    [[nodiscard]] bool removeRoute(const Ip4::Ip4Route &route) const;

    [[nodiscard]] bool addRoute(const Ip4::Ip4Route &route) const;

    bool setAccessMode(Util::Io::File::AccessMode accessMode) const;

    [[nodiscard]] bool isReadyToRead() const;

private:
    /**
     * Default Constructor.
     */
    Socket(int32_t fileDescriptor, Type type);

    int32_t fileDescriptor;
    Type type;
};

}

#endif
