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

#ifndef __EhciDevice_include__
#define __EhciDevice_include__

#include "StorageDevice.h"
#include "lib/String.h"

#include <cstdint>
#include <devices/usb/ehci/Ehci.h>

/**
 * Implementation of StorageDevice for a device, that is controlled by an EhciController (see devices/usb/Ehci.h).
 */
class EhciDevice : public StorageDevice {

private:
    Ehci &controller;
    uint8_t ehciDiskNumber;

public:
    /**
     * Constructor.
     *
     * @param controller A reference to the controller, that controls this device.
     * @param ehciDiskNumber The slot, that the device takes in the controller's device-array.
     * @param name The name
     */
    EhciDevice(Ehci &controller, uint8_t ehciDiskNumber, String name);

    /**
     * Copy-constructor.
     */
    EhciDevice(EhciDevice &copy) = delete;

    /**
     * Destructor.
     */
    ~EhciDevice() override = default;

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