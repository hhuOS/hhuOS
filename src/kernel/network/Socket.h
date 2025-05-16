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

#ifndef HHUOS_SOCKET_H
#define HHUOS_SOCKET_H

#include <stdint.h>

#include "filesystem/Node.h"
#include "lib/util/network/Socket.h"

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
    explicit Socket(NetworkModule &networkModule, Util::Network::Socket::Type type);

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

    [[nodiscard]] bool isBound() const;

    void setTimeout(uint32_t timeout);

    bool control(uint32_t request, const Util::Array<uint32_t> &parameters) override;

    virtual bool send(const Util::Network::Datagram &datagram) = 0;

    virtual Util::Network::Datagram* receive() = 0;

protected:

    Util::Network::NetworkAddress *bindAddress{};
    uint32_t timeout = 0;

private:

    NetworkModule &networkModule;
    Util::Network::Socket::Type type;
};

}

#endif
