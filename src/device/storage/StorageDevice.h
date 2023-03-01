/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_STORAGEDEVICE_H
#define HHUOS_STORAGEDEVICE_H

#include <cstdint>

namespace Device::Storage {

class StorageDevice {

public:
    /**
     * Default Constructor.
     */
    StorageDevice() = default;

    /**
     * Copy Constructor.
     */
    StorageDevice(const StorageDevice &other) = delete;

    /**
     * Assignment operator.
     */
    StorageDevice &operator=(const StorageDevice &other) = delete;

    /**
     * Destructor.
     */
    virtual ~StorageDevice() = default;

    /**
     * Get the size of a single sector in bytes.
     */
    virtual uint32_t getSectorSize() = 0;

    /**
     * Get the amount of sectors, that the device consists of.
     */
    virtual uint64_t getSectorCount() = 0;

    /**
     * Read sectors from the device.
     *
     * @param buffer The buffer, where the read data will be stored in
     * @param startSector The startSector number
     * @param sectorCount The amount of sectors, that will be read
     *
     * @return The amount of read sectors
     */
    virtual uint32_t read(uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) = 0;

    /**
     * Write sectors to the device.
     *
     * @param buffer Contains the data, that will be written to the device
     * @param startSector The startSector number
     * @param sectorCount The amount of sectors, that will be written
     *
     * @return The amount of written sectors
     */
    virtual uint32_t write(const uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) = 0;
};

}

#endif