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

#include <cstdint>

#include "device/storage/StorageDevice.h"
#include "lib/util/memory/Address.h"
#include "lib/util/async/Spinlock.h"

#ifndef HHUOS_VIRTUALDISKDRIVE_H
#define HHUOS_VIRTUALDISKDRIVE_H

namespace Device::Storage {

/**
 * Implementation of StorageDevice for a virtual device, that exists only in memory.
 */
class VirtualDiskDrive : public StorageDevice {

public:
    /**
     * Constructor.
     */
    VirtualDiskDrive(uint32_t sectorSize, uint32_t sectorCount);

    /**
     * Constructor.
     */
    VirtualDiskDrive(void *address, uint32_t sectorSize, uint32_t sectorCount);

    /**
     * Copy Constructor.
     */
    VirtualDiskDrive(const VirtualDiskDrive &other) = delete;

    /**
     * Assignment operator.
     */
    VirtualDiskDrive &operator=(const VirtualDiskDrive &other) = delete;

    /**
     * Destructor.
     */
    ~VirtualDiskDrive() override;

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

    Util::Async::Spinlock ioLock;

    Util::Memory::Address<uint32_t> address;
    bool freeAddress;

    uint32_t sectorSize;
    uint32_t sectorCount;
};

}

#endif