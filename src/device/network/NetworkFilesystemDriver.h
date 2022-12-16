/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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
 */

#ifndef HHUOS_NETWORKFILESYSTEMDRIVER_H
#define HHUOS_NETWORKFILESYSTEMDRIVER_H

#include "filesystem/memory/MemoryDriver.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device

namespace Device::Network {

class NetworkFilesystemDriver : public Filesystem::Memory::MemoryDriver {

public:
    /**
     * Copy Constructor.
     */
    NetworkFilesystemDriver(const NetworkFilesystemDriver &other) = delete;

    /**
     * Assignment operator.
     */
    NetworkFilesystemDriver &operator=(const NetworkFilesystemDriver &other) = delete;

    /**
     * Destructor.
     */
    ~NetworkFilesystemDriver() override = default;

    static bool mount(NetworkDevice &device);

private:
    /**
     * Default Constructor.
     */
    explicit NetworkFilesystemDriver(NetworkDevice &device);

    NetworkDevice &device;
};

}

#endif
