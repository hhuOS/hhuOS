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
 *
 * The IDE driver is based on a bachelor's thesis, written by Tim Laurischkat.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-tilau101
 */

#ifndef HHUOS_IDEDEVICE_H
#define HHUOS_IDEDEVICE_H

#include <cstdint>

#include "device/storage/StorageDevice.h"
#include "IdeController.h"

namespace Device::Storage {

class IdeDevice : public StorageDevice {

public:
    /**
     * Constructor.
     */
    IdeDevice(IdeController &controller, const IdeController::DeviceInfo &deviceInfo);

    /**
     * Copy Constructor.
     */
    IdeDevice(const IdeDevice &other) = delete;

    /**
     * Assignment operator.
     */
    IdeDevice &operator=(const IdeDevice &other) = delete;

    /**
     * Destructor.
     */
    ~IdeDevice() override = default;

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

    IdeController &controller;
    IdeController::DeviceInfo info;
};

}

#endif
