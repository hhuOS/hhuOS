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

#include "StorageService.h"

#include "device/storage/PartitionHandler.h"
#include "device/storage/Partition.h"
#include "device/storage/StorageDevice.h"
#include "kernel/log/Logger.h"
#include "lib/util/Exception.h"

namespace Kernel {

Logger StorageService::log = Logger::get("Storage");
Util::Data::HashMap<Util::Memory::String, uint32_t> StorageService::nameMap;

StorageService::~StorageService() {
    for (const auto &key : deviceMap.keys()) {
        delete deviceMap.get(key);
    }
}

Util::Memory::String StorageService::registerDevice(Device::Storage::StorageDevice *device, const Util::Memory::String &deviceClass) {
    lock.acquire();
    if (!nameMap.containsKey(deviceClass)) {
        nameMap.put(deviceClass, 0);
    }

    auto value = nameMap.get(deviceClass);
    auto name = Util::Memory::String::format("%s%u", static_cast<char*>(deviceClass), value);
    deviceMap.put(name, device);
    nameMap.put(deviceClass, value + 1);

    log.info("Registered device [%s]",static_cast<char*>(name));

    if (lock.getDepth() == 1) {
        log.info("Scanning device [%s] for partitions", static_cast<char *>(name));
        auto partitionReader = Device::Storage::PartitionHandler(*device);
        for (const auto &info: partitionReader.readPartitionTable()) {
            auto *partition = new Device::Storage::Partition(*device, info.startSector, info.sectorCount);
            registerDevice(partition, name + "p");
        }
    }

    lock.release();
    return name;
}

Device::Storage::StorageDevice &StorageService::getDevice(const Util::Memory::String &deviceName) {
    lock.acquire();
    if (!deviceMap.containsKey(deviceName)) {
        lock.release();
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "StorageService: Device not found!");
    }

    auto &result = *deviceMap.get(deviceName);
    lock.release();

    return result;
}

bool StorageService::isDeviceRegistered(const Util::Memory::String &deviceName) {
    lock.acquire();
    auto result = deviceMap.containsKey(deviceName);
    lock.release();

    return result;
}

}