/*
 * Copyright (C) 2019 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef HHUOS_NETWORKDEVICE_H
#define HHUOS_NETWORKDEVICE_H

#include "kernel/interrupt/InterruptHandler.h"
#include <cstdint>

/**
 * Interface for network cards
 *
 * @author Fabian Ruhland
 * @date 2019
 */
class NetworkDevice : public Kernel::InterruptHandler {
public:

    /**
     * Constructor.
     */
    NetworkDevice() = default;

    /**
     * Copy-constructor.
     */
    NetworkDevice(const NetworkDevice &copy) = delete;

    /**
     * Assignment operator.
     */
    NetworkDevice& operator=(const NetworkDevice & other) = delete;

    /**
     * Destructor.
     */
    ~NetworkDevice() override = default;

    /**
     * Send a buffer.
     *
     * @param address The buffer's address
     * @param length The buffer's length
     */
    virtual void sendPacket(void *address, uint16_t length) = 0;

    /**
     * Read the MAC-address into a given buffer.
     *
     * @param buf The buffer to read the MAC-address into.
     */
    virtual void getMacAddress(uint8_t *buf) = 0;

};

#endif
