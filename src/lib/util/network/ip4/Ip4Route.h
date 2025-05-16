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

#ifndef HHUOS_IP4ROUTE_H
#define HHUOS_IP4ROUTE_H

#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/base/String.h"
#include "Ip4SubnetAddress.h"

namespace Util::Network::Ip4 {

class Ip4Route {

public:
    /**
     * Default Constructor.
     */
    Ip4Route() = default;

    /**
     * Constructor.
     */
    Ip4Route(const Ip4SubnetAddress &targetAddress, const Util::String &deviceIdentifier);

    /**
     * Constructor.
     */
    Ip4Route(const Ip4Address &sourceAddress, const Ip4SubnetAddress &targetAddress, const String &deviceIdentifier);

    /**
     * Constructor.
     */
    Ip4Route(const Ip4SubnetAddress &targetAddress, const Ip4Address &nextHop, const String &deviceIdentifier);

    /**
     * Constructor.
     */
    Ip4Route(const Ip4Address &sourceAddress, const Ip4SubnetAddress &targetAddress, const Ip4Address &nextHop, const String &deviceIdentifier);

    /**
     * Copy Constructor.
     */
    Ip4Route(const Ip4Route &other) = default;

    /**
     * Assignment operator.
     */
    Ip4Route &operator=(const Ip4Route &other) = default;

    /**
     * Destructor.
     */
    ~Ip4Route() = default;

    bool operator==(const Ip4Route &other) const;

    bool operator!=(const Ip4Route &other) const;

    [[nodiscard]] const Ip4Address& getSourceAddress() const;

    [[nodiscard]] Ip4SubnetAddress getTargetAddress() const;

    [[nodiscard]] const String& getDeviceIdentifier() const;

    [[nodiscard]] bool hasNextHop() const;

    [[nodiscard]] const Ip4Address& getNextHop() const;

    [[nodiscard]] bool isValid();

private:

    Ip4Address sourceAddress{};
    Ip4SubnetAddress targetAddress{};
    Ip4Address nextHop{};
    String deviceIdentifier{};

    bool nextHopValid{};
};

}

#endif
