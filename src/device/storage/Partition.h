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

#ifndef HHUOS_PARTITION_H
#define HHUOS_PARTITION_H

#include <cstdint>

#include "StorageDevice.h"

namespace Device::Storage {

class Partition : public StorageDevice {

public:
    /**
     * Constructor.
     */
    Partition(StorageDevice &parentDevice, uint32_t startSector, uint32_t sectorCount);

    /**
     * Copy Constructor.
     */
    Partition(const Partition &other) = delete;

    /**
     * Assignment operator.
     */
    Partition &operator=(const Partition &other) = delete;

    /**
     * Destructor.
     */
    ~Partition() override = default;

    /**
     * Overriding function from StorageDevice.
     */
    uint32_t getSectorSize() override;

    /**
     * Overriding function from StorageDevice.
     */
    uint64_t getSectorCount() override;

    /**
     * Overriding function from StorageDevice.
     */
    uint32_t read(uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) override;

    /**
     * Overriding function from StorageDevice.
     */
    uint32_t write(const uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) override;

private:

    StorageDevice &parentDevice;
    uint32_t startSector;
    uint32_t sectorCount;
};

}

#endif