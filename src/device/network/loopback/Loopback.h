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

#ifndef HHUOS_LOOPBACK_H
#define HHUOS_LOOPBACK_H

#include <stdint.h>

#include "device/network/NetworkDevice.h"
#include "lib/util/async/Spinlock.h"

namespace Device::Network {

class Loopback : public NetworkDevice {

public:
    /**
     * Default Constructor.
     */
    Loopback() = default;

    /**
     * Copy Constructor.
     */
    Loopback(const Loopback &other) = delete;

    /**
     * Assignment operator.
     */
    Loopback &operator=(const Loopback &other) = delete;

    /**
     * Destructor.
     */
    ~Loopback() override = default;

    /**
     * Overriding function from NetworkDevice.
     */
    [[nodiscard]] Util::Network::MacAddress getMacAddress() const override;

protected:

    /**
     * Overriding function from NetworkDevice.
     */
    void handleOutgoingPacket(const uint8_t *packet, uint32_t length) override;

private:

    Util::Async::Spinlock lock;
};

}

#endif
