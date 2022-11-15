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

#ifndef HHUOS_NETWORKDEVICE_H
#define HHUOS_NETWORKDEVICE_H

#include "kernel/interrupt/InterruptHandler.h"
#include "lib/util/stream/FilterInputStream.h"
#include "lib/util/stream/OutputStream.h"
#include "network/MacAddress.h"

namespace Device::Network {

/**
 * Interface for network cards
 */
class NetworkDevice : public Util::Stream::FilterInputStream, public Util::Stream::OutputStream {

public:
    /**
     * Constructor.
     */
    explicit NetworkDevice(Util::Stream::InputStream &inputStream);

    /**
     * Copy-constructor.
     */
    NetworkDevice(const NetworkDevice &copy) = delete;

    /**
     * Assignment operator.
     */
    NetworkDevice &operator=(const NetworkDevice &other) = delete;

    /**
     * Destructor.
     */
    ~NetworkDevice() override = default;

    /**
     * Read the MAC-address into a given buffer.
     *
     * @param buf The buffer to read the MAC-address into.
     */
    virtual ::Network::MacAddress getMacAddress() = 0;

};

}

#endif