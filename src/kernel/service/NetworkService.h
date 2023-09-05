/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The network stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_NETWORKSERVICE_H
#define HHUOS_NETWORKSERVICE_H

#include <cstdint>

#include "Service.h"
#include "kernel/network/NetworkStack.h"
#include "lib/util/base/String.h"
#include "lib/util/network/Socket.h"
#include "device/network/NetworkDevice.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/collection/HashMap.h"

namespace Util {
namespace Network {
class MacAddress;
}  // namespace Network
}  // namespace Util

namespace Kernel {
class Logger;

class NetworkService : public Service {

public:
    /**
     * Default Constructor.
     */
    NetworkService();

    /**
     * Copy Constructor.
     */
    NetworkService(const NetworkService &other) = delete;

    /**
     * Assignment operator.
     */
    NetworkService &operator=(const NetworkService &other) = delete;

    /**
     * Destructor.
     */
    ~NetworkService() override = default;

    void initializeLoopback();

    Util::String registerNetworkDevice(Device::Network::NetworkDevice *device, const Util::String &deviceClass);

    bool isNetworkDeviceRegistered(const Util::String &identifier);

    Device::Network::NetworkDevice& getNetworkDevice(const Util::String &identifier);

    Device::Network::NetworkDevice& getNetworkDevice(const Util::Network::MacAddress &address);

    Network::NetworkStack& getNetworkStack();

    int32_t createSocket(Util::Network::Socket::Type socketType);

    static const constexpr uint8_t SERVICE_ID = 8;

private:

    Util::Async::Spinlock lock;
    Util::HashMap<Util::String, Device::Network::NetworkDevice*> deviceMap;
    Network::NetworkStack networkStack;

    static Logger log;
    static Util::HashMap<Util::String, uint32_t> nameMap;
};

}


#endif
