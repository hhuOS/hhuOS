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

#ifndef HHUOS_STORAGESERVICE_H
#define HHUOS_STORAGESERVICE_H

#include <cstdint>

#include "Service.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/async/ReentrantSpinlock.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/base/String.h"
#include "device/storage/StorageDevice.h"

namespace Kernel {
class Logger;

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

    Util::String registerDevice(Device::Storage::StorageDevice *device, const Util::String &deviceClass);

    Device::Storage::StorageDevice& getDevice(const Util::String &deviceName);

    bool isDeviceRegistered(const Util::String &deviceName);

    static const constexpr uint8_t SERVICE_ID = 5;

private:

    Util::Async::ReentrantSpinlock lock;
    Util::HashMap<Util::String, Device::Storage::StorageDevice*> deviceMap;

    static Logger log;
    static Util::HashMap<Util::String, uint32_t> nameMap;

};

}

#endif
