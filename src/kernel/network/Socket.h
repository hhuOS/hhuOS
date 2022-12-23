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

#ifndef HHUOS_SOCKET_H
#define HHUOS_SOCKET_H

#include <cstdint>

#include "filesystem/core/Node.h"
#include "lib/util/data/Array.h"

namespace Kernel {
namespace Network {
class NetworkModule;
}  // namespace Network
}  // namespace Kernel

namespace Util {
namespace Network {
class NetworkAddress;
class Datagram;
}  // namespace Network
}  // namespace Util

namespace Kernel::Network {

class Socket : public Filesystem::Node {

public:
    /**
     * Constructor.
     */
    explicit Socket(NetworkModule &networkModule);

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
    ~Socket() override;

    void bind(const Util::Network::NetworkAddress &address);

    [[nodiscard]] const Util::Network::NetworkAddress& getAddress() const;

    bool control(uint32_t request, const Util::Data::Array<uint32_t> &parameters) override;

    virtual bool send(const Util::Network::Datagram &datagram) = 0;

    virtual Util::Network::Datagram* receive() = 0;

    Util::Network::NetworkAddress *bindAddress{};

private:

    NetworkModule &networkModule;
};

}

#endif
