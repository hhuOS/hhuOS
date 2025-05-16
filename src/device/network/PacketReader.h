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

#ifndef HHUOS_PACKETREADER_H
#define HHUOS_PACKETREADER_H

#include "lib/util/async/Runnable.h"

namespace Device::Network {

class NetworkDevice;

class PacketReader : public Util::Async::Runnable {

public:
    /**
     * Default Constructor.
     */
    explicit PacketReader(NetworkDevice &networkDevice);

    /**
     * Copy Constructor.
     */
    PacketReader(const PacketReader &other) = delete;

    /**
     * Assignment operator.
     */
    PacketReader &operator=(const PacketReader &other) = delete;

    /**
     * Destructor.
     */
    ~PacketReader() override = default;

    void run() override;

private:

    Device::Network::NetworkDevice &networkDevice;
};

}

#endif
