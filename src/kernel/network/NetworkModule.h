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

#ifndef HHUOS_NETWORKMODULE_H
#define HHUOS_NETWORKMODULE_H

#include <stdint.h>

#include "lib/util/collection/HashMap.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/collection/ArrayList.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device
namespace Util {
namespace Network {
class NetworkAddress;
}  // namespace Network

namespace Io {
class ByteArrayInputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel {
namespace Network {

class Socket;

class NetworkModule {

public:

    struct LayerInformation {
        const Util::Network::NetworkAddress &sourceAddress;
        const Util::Network::NetworkAddress &destinationAddress;
        const uint32_t payloadLength;
    };

    /**
     * Default Constructor.
     */
    NetworkModule() = default;

    /**
     * Copy Constructor.
     */
    NetworkModule(const NetworkModule &other) = delete;

    /**
     * Assignment operator.
     */
    NetworkModule &operator=(const NetworkModule &other) = delete;

    /**
     * Destructor.
     */
    ~NetworkModule() = default;

    bool isNextLayerTypeSupported(uint32_t protocolId);

    void registerNextLayerModule(uint32_t protocolId, NetworkModule &module);

    virtual bool registerSocket(Socket &socket);

    virtual void deregisterSocket(Socket &socket);

    virtual void readPacket(Util::Io::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) = 0;

protected:

    void invokeNextLayerModule(uint32_t protocolId, LayerInformation information, Util::Io::ByteArrayInputStream &stream, Device::Network::NetworkDevice &device);

    Util::Async::Spinlock socketLock;
    Util::ArrayList<Socket*> socketList;

private:

    Util::HashMap<uint32_t, NetworkModule*> nextLayerModules;
};

}
}

#endif
