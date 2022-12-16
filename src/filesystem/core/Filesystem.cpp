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

#include "lib/util/file/File.h"
#include "kernel/system/System.h"
#include "kernel/service/StorageService.h"
#include "lib/util/reflection/InstanceFactory.h"
#include "PhysicalDriver.h"
#include "Filesystem.h"
#include "filesystem/core/Driver.h"
#include "filesystem/core/Node.h"
#include "filesystem/core/VirtualDriver.h"
#include "lib/util/file/Type.h"

namespace Filesystem {
namespace Memory {
class MemoryDriver;
}  // namespace Memory

bool Filesystem::mount(const Util::Memory::String &deviceName, const Util::Memory::String &targetPath, const Util::Memory::String &driverName) {
    auto &storageService = Kernel::System::getService<Kernel::StorageService>();
    if (!storageService.isDeviceRegistered(deviceName)) {
        return false;
    }

    lock.acquire();

    auto parsedPath = Util::File::File::getCanonicalPath(targetPath) + Util::File::File::SEPARATOR;
    auto *targetNode = getNode(parsedPath);
    if (targetNode == nullptr) {
        if (mountPoints.size() != 0) {
            return lock.releaseAndReturn(false);
        }
    }

    auto &device = storageService.getDevice(deviceName);
    auto *driver = INSTANCE_FACTORY_CREATE_INSTANCE(PhysicalDriver, driverName);
    if (driver == nullptr || !driver->mount(device)) {
        delete driver;
        return lock.releaseAndReturn(false);
    }

    if (mountPoints.containsKey(parsedPath)) {
        return lock.releaseAndReturn(false);
    }

    mountPoints.put(parsedPath, driver);
    mountInformation.put(parsedPath, {deviceName, targetPath, driverName});
    return lock.releaseAndReturn(true);
}

bool Filesystem::mountVirtualDriver(const Util::Memory::String &targetPath, VirtualDriver *driver) {
    auto parsedPath = Util::File::File::getCanonicalPath(targetPath) + Util::File::File::SEPARATOR;

    lock.acquire();

    auto *targetNode = getNode(parsedPath);
    if (targetNode == nullptr) {
        if (mountPoints.size() != 0) {
            return lock.releaseAndReturn(false);
        }
    }

    delete targetNode;

    if (mountPoints.containsKey(parsedPath)) {
        return lock.releaseAndReturn(false);
    }

    mountPoints.put(parsedPath, driver);
    mountInformation.put(parsedPath, {"Virtual", targetPath, "VirtualDriver"});
    return lock.releaseAndReturn(true);
}

Memory::MemoryDriver& Filesystem::getVirtualDriver(const Util::Memory::String &path) {
    lock.acquire();
    auto parsedPath = Util::File::File::getCanonicalPath(path) + Util::File::File::SEPARATOR;
    auto *driver = mountPoints.get(parsedPath);

    return lock.releaseAndReturn<Memory::MemoryDriver&>(*reinterpret_cast<Memory::MemoryDriver*>(driver));
}

bool Filesystem::unmount(const Util::Memory::String &path) {
    auto parsedPath = Util::File::File::getCanonicalPath(path) + Util::File::File::SEPARATOR;

    lock.acquire();

    auto *targetNode = getNode(parsedPath);
    if (targetNode == nullptr) {
        if (path != "/") {
            return lock.releaseAndReturn(false);
        }
    }

    delete targetNode;

    for(const Util::Memory::String &key : mountPoints.keys()) {
        if(key.beginsWith(parsedPath)) {
            if(key != parsedPath) {
                return lock.releaseAndReturn(false);
            }
        }
    }

    if(mountPoints.containsKey(parsedPath)) {
        mountInformation.remove(parsedPath);
        delete mountPoints.remove(parsedPath);
        return lock.releaseAndReturn(true);
    }

    return lock.releaseAndReturn(false);
}

bool Filesystem::createFilesystem(const Util::Memory::String &deviceName, const Util::Memory::String &driverName) {
    auto &storageService = Kernel::System::getService<Kernel::StorageService>();
    if (!storageService.isDeviceRegistered(deviceName)) {
        return false;
    }

    lock.acquire();

    auto &device = storageService.getDevice(deviceName);
    auto *driver = INSTANCE_FACTORY_CREATE_INSTANCE(PhysicalDriver, driverName);
    auto result = driver->createFilesystem(device);

    delete driver;
    return lock.releaseAndReturn(result);
}

Node* Filesystem::getNode(const Util::Memory::String &path) {
    auto parsedPath = Util::File::File::getCanonicalPath(path);
    lock.acquire();

    auto *driver = getMountedDriver(parsedPath);
    if (driver == nullptr) {
        return lock.releaseAndReturn(nullptr);
    }

    Node *ret = driver->getNode(parsedPath);
    return lock.releaseAndReturn(ret);
}

bool Filesystem::createFile(const Util::Memory::String &path) {
    auto parsedPath = Util::File::File::getCanonicalPath(path);
    lock.acquire();

    auto *driver = getMountedDriver(parsedPath);
    if (driver == nullptr) {
        return lock.releaseAndReturn(false);
    }

    bool ret = driver->createNode(parsedPath, Util::File::REGULAR);
    return lock.releaseAndReturn(ret);
}

bool Filesystem::createDirectory(const Util::Memory::String &path) {
    auto parsedPath = Util::File::File::getCanonicalPath(path);
    lock.acquire();

    auto *driver = getMountedDriver(parsedPath);
    if (driver == nullptr) {
        return lock.releaseAndReturn(false);
    }

    bool ret = driver->createNode(parsedPath, Util::File::DIRECTORY);
    return lock.releaseAndReturn(ret);
}

bool Filesystem::deleteFile(const Util::Memory::String &path) {
    auto parsedPath = Util::File::File::getCanonicalPath(path);
    lock.acquire();

    for (const Util::Memory::String &key : mountPoints.keys()) {
        if (key.beginsWith(parsedPath)) {
            lock.release();
            return false;
        }
    }

    auto *driver = getMountedDriver(parsedPath);
    if (driver == nullptr) {
        return lock.releaseAndReturn(false);
    }

    bool ret = driver->deleteNode(parsedPath);
    return lock.releaseAndReturn<bool>(ret);
}

Driver* Filesystem::getMountedDriver(Util::Memory::String &path) {
    if (!path.endsWith(Util::File::File::SEPARATOR)) {
        path += Util::File::File::SEPARATOR;
    }


    lock.acquire();

    Util::Memory::String ret;
    for (const Util::Memory::String &currentString: mountPoints.keys()) {
        if (path.beginsWith(currentString)) {
            if (currentString.length() > ret.length()) {
                ret = currentString;
            }
        }
    }

    if (ret.isEmpty()) {
        return lock.releaseAndReturn(nullptr);
    }

    path = path.substring(ret.length(), path.length() - 1);
    return lock.releaseAndReturn(mountPoints.get(ret));
}

Util::Data::Array<MountInformation> Filesystem::getMountInformation() {
    lock.acquire();
    return lock.releaseAndReturn(mountInformation.values());
}

bool MountInformation::operator!=(const MountInformation &other) const {
    return target == other.target;
}

}