/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
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

#ifndef HHUOS_STORAGESERVICE_H
#define HHUOS_STORAGESERVICE_H

#include "Service.h"
#include "lib/util/data/HashMap.h"
#include "device/storage/StorageDevice.h"
#include "lib/util/async/Spinlock.h"
#include "kernel/log/Logger.h"

namespace Kernel {

class StorageService : public Service {

public:
    /**
     * Constructor.
     */
    StorageService() = default;

    /**
     * Copy Constructor.
     */
    StorageService(const StorageService &copy) = delete;

    /**
     * Assignment operator.
     */
    StorageService &operator=(const StorageService &other) = delete;

    /**
     * Destructor.
     */
    ~StorageService() override;

    Util::Memory::String registerDevice(Device::Storage::StorageDevice *device, const Util::Memory::String &deviceClass);

    Device::Storage::StorageDevice& getDevice(const Util::Memory::String &deviceName);

    bool isDeviceRegistered(const Util::Memory::String &deviceName);

    static const constexpr uint8_t SERVICE_ID = 6;

private:

    Util::Async::Spinlock lock;
    Util::Data::HashMap<Util::Memory::String, Device::Storage::StorageDevice*> deviceMap;

    static Logger log;
    static Util::Data::HashMap<Util::Memory::String, uint32_t> nameMap;

};

}

#endif
