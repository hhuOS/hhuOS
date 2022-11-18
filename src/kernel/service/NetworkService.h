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

    Network::Ethernet::EthernetModule& getEthernetModule();

    Network::Arp::ArpModule& getArpModule();

    static const constexpr uint8_t SERVICE_ID = 8;

private:

    Network::Ethernet::EthernetModule ethernetModule;
    Network::Arp::ArpModule arpModule;
};

}


#endif
