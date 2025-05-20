/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "StorageService.h"

#include "device/storage/PartitionHandler.h"
#include "device/storage/Partition.h"
#include "device/storage/StorageDevice.h"
#include "kernel/log/Log.h"
#include "lib/util/base/Panic.h"
#include "lib/util/collection/Array.h"

namespace Kernel {

Util::HashMap<Util::String, uint32_t> StorageService::nameMap;

StorageService::~StorageService() {
    for (const auto &key : deviceMap.keys()) {
        delete deviceMap.get(key);
    }
}

Util::String StorageService::registerDevice(Device::Storage::StorageDevice *device, const Util::String &deviceClass) {
    lock.acquire();
    if (!nameMap.containsKey(deviceClass)) {
        nameMap.put(deviceClass, 0);
    }

    auto value = nameMap.get(deviceClass);
    auto name = Util::String::format("%s%u", static_cast<const char*>(deviceClass), value);
    deviceMap.put(name, device);
    nameMap.put(deviceClass, value + 1);

    LOG_INFO("Registered device [%s]",static_cast<const char*>(name));

    if (lock.getDepth() == 1) {
        LOG_INFO("Scanning device [%s] for partitions", static_cast<const char *>(name));
        auto partitionReader = Device::Storage::PartitionHandler(*device);
        for (const auto &info: partitionReader.readPartitionTable()) {
            auto *partition = new Device::Storage::Partition(*device, info.startSector, info.sectorCount);
            registerDevice(partition, name + "p");
        }
    }

    lock.release();
    return name;
}

Device::Storage::StorageDevice &StorageService::getDevice(const Util::String &deviceName) {
    lock.acquire();
    if (!deviceMap.containsKey(deviceName)) {
        lock.release();
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "StorageService: Device not found!");
    }

    auto &result = *deviceMap.get(deviceName);
    lock.release();

    return result;
}

bool StorageService::isDeviceRegistered(const Util::String &deviceName) {
    lock.acquire();
    auto result = deviceMap.containsKey(deviceName);
    lock.release();

    return result;
}

}