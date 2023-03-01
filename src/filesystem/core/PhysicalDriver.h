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

#ifndef HHUOS_PHYSICAL_DRIVER_H
#define HHUOS_PHYSICAL_DRIVER_H

#include "device/storage/StorageDevice.h"
#include "lib/util/reflection/Prototype.h"
#include "Driver.h"

namespace Filesystem {

class PhysicalDriver : public Driver, public Util::Reflection::Prototype {

public:
    /**
     * Default Constructor.
     */
    PhysicalDriver() = default;

    /**
     * Copy Constructor.
     */
    PhysicalDriver(const PhysicalDriver &other) = delete;

    /**
     * Assignment operator.
     */
    PhysicalDriver &operator=(const PhysicalDriver &other) = delete;

    /**
     * Destructor.
     */
    ~PhysicalDriver() override = default;

    /**
     * Mount a device.
     * After this function has succeeded, the driver must be ready to process requests for this device.
     *
     * @param device The device
     *
     * @return True on success
     */
    virtual bool mount(Device::Storage::StorageDevice &device) = 0;

    /**
     * Format a device.
     *
     * @param device The device
     *
     * @return True on success
     */
    virtual bool createFilesystem(Device::Storage::StorageDevice &device) = 0;
};

}

#endif
