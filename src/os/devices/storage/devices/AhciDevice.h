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

#ifndef __AhciDevice_include__
#define __AhciDevice_include__

#include "StorageDevice.h"
#include "lib/String.h"

#include <cstdint>
#include <devices/storage/controller/Ahci.h>

/**
 * Implementation of StorageDevice for a Device, that is controlled by an AhciController (see devices/storage/controller/Ahci.h).
 */
class AhciDevice : public StorageDevice {

private:
    Ahci &controller;
    uint8_t ahciDiskNumber;

    Ahci::AhciDeviceInfo deviceInfo;

public:
    /**
     * Constructor.
     *
     * @param controller A reference to the controller, that controls this device.
     * @param ahciDiskNumber The slot, that the device takes in the controller's device-array.
     * @param name The name
     */
    AhciDevice(Ahci &controller, uint8_t ahciDiskNumber, String name);

    /**
     * Copy-constructor.
     */
    AhciDevice(AhciDevice &copy) = delete;

    /**
     * Destructor.
     */
    ~AhciDevice() override = default;

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