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

#include "StorageService.h"

namespace Kernel {

Logger StorageService::log = Logger::get("Storage");
Util::Data::HashMap<Util::Memory::String, uint32_t> StorageService::nameMap;

StorageService::~StorageService() {
    for (const auto &key : deviceMap.keySet()) {
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
    lock.release();

    log.info("Registered device %s",static_cast<char*>(name));
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