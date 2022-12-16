/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#ifndef HHUOS_NETWORKSERVICE_H
#define HHUOS_NETWORKSERVICE_H

#include <cstdint>

#include "Service.h"
#include "network/NetworkStack.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"
#include "lib/util/memory/String.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device
namespace Network {
class MacAddress;
namespace Ip4 {
class Ip4Route;
}  // namespace Ip4
}  // namespace Network

namespace Kernel {

class NetworkService : public Service {

public:
    /**
     * Default Constructor.
     */
    NetworkService() = default;

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

    void registerNetworkDevice(Device::Network::NetworkDevice *device);

    void registerIp4Route(const Network::Ip4::Ip4Route &route);

    void setDefaultRoute(const Network::Ip4::Ip4Route &route);

    Device::Network::NetworkDevice& getNetworkDevice(const Util::Memory::String &identifier);

    Device::Network::NetworkDevice& getNetworkDevice(const ::Network::MacAddress &address);

    ::Network::NetworkStack& getNetworkStack();

    static const constexpr uint8_t SERVICE_ID = 8;

private:

    Util::Data::ArrayList<Device::Network::NetworkDevice*> devices;
    ::Network::NetworkStack networkStack;
};

}


#endif
