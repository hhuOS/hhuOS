/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef __VirtualDiskDrive_include__
#define __VirtualDiskDrive_include__

#include "StorageDevice.h"
#include "lib/String.h"

#include <cstdint>

/**
 * Implementation of StorageDevice for a virtual device, that exists only in RAM.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class VirtualDiskDrive : public StorageDevice {

private:
    static uint32_t nameCounter;

    uint32_t sectorSize;
    uint32_t sectorCount;
    String name;

    uint8_t *buffer = nullptr;

public:
    /**
     * Constructor.
     *
     * @param sectorSize The virtual size of a sector on the virtual disk
     * @param sectorCount The amount of sectors, that the virtual disk shall consist of
     * @param name The name
     */
    VirtualDiskDrive(uint32_t sectorSize, uint32_t sectorCount);

    /**
     * Copy-constructor.
     */
    VirtualDiskDrive(VirtualDiskDrive &copy) = delete;

    /**
     * Destructor.
     */
    ~VirtualDiskDrive() override;

    /**
     * Overriding function from StorageDevice.
     */
    String getHardwareName() override;

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
    bool read(uint8_t *buff, uint32_t sector, uint32_t count) override;

    /**
     * Overriding function from StorageDevice.
     */
    bool write(const uint8_t *buff, uint32_t sector, uint32_t count) override;
};

#endif