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

#include "network/ethernet/EthernetModule.h"
#include "Service.h"
#include "network/arp/ArpModule.h"
#include "network/ip4/Ip4Module.h"
#include "network/NetworkStack.h"

namespace Kernel {

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

    void registerNetworkDevice(Device::Network::NetworkDevice *device);

    Device::Network::NetworkDevice& getNetworkDevice(const Util::Memory::String &identifier);

    ::Network::NetworkStack& getNetworkStack();

    static const constexpr uint8_t SERVICE_ID = 8;

private:

    Util::Data::ArrayList<Device::Network::NetworkDevice*> devices;
    ::Network::NetworkStack networkStack;
};

}


#endif
