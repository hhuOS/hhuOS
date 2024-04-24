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
 * The AHCI driver is based on a bachelor's thesis, written by Manuel Demetrio Angelescu.
 * The original source code can be found here: https://github.com/m8nu/hhuOS
 */

#ifndef HHUOS_AHCIDEVICE_H
#define HHUOS_AHCIDEVICE_H

#include <stdint.h>

#include "AhciController.h"
#include "device/storage/StorageDevice.h"

namespace Device::Storage {

class AhciDevice : public Device::Storage::StorageDevice {

public:
    /**
     * Default Constructor.
     */
    AhciDevice(uint32_t portNumber, AhciController::DeviceInfo *deviceInfo, AhciController &controller);

    /**
     * Copy Constructor.
     */
    AhciDevice(const AhciDevice &other) = delete;

    /**
     * Assignment operator.
     */
    AhciDevice &operator=(const AhciDevice &other) = delete;

    /**
     * Destructor.
     */
    ~AhciDevice() override;

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

    const uint32_t portNumber;
    const AhciController::DeviceInfo &info;
    AhciController &controller;

};

}

#endif
