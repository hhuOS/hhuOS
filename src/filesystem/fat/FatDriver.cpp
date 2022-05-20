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

#include "FatDriver.h"
#include "filesystem/fat/ff/source/ff.h"
#include "FatNode.h"

namespace Filesystem::Fat {

Util::Memory::AtomicBitmap FatDriver::volumeIdAllocator(FF_VOLUMES);
Util::Data::HashMap<uint32_t, Device::Storage::StorageDevice*> FatDriver::deviceMap;

FatDriver::~FatDriver() {
    f_mount(nullptr, static_cast<const char*>(Util::Memory::String::format("%u", volumeId)), 1);
    volumeIdAllocator.unset(volumeId);
}

Device::Storage::StorageDevice& FatDriver::getStorageDevice(uint8_t volumeId) {
    return *deviceMap.get(volumeId);
}

bool FatDriver::mount(Device::Storage::StorageDevice &device) {
    volumeId = volumeIdAllocator.findAndSet();
    if (volumeId == Util::Memory::AtomicBitmap::INVALID_INDEX) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Maximum amount of fat volumes reached!");
    }

    deviceMap.put(volumeId, &device);

    auto result = f_mount(&fatVolume, static_cast<const char*>(Util::Memory::String::format("%u", volumeId)), 1);
    return result == FR_OK;
}

bool FatDriver::createFilesystem(Device::Storage::StorageDevice &device) {
    volumeId = volumeIdAllocator.findAndSet();
    if (volumeId == Util::Memory::AtomicBitmap::INVALID_INDEX) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Maximum amount of fat volumes reached!");
    }

    deviceMap.put(volumeId, &device);
    auto *work = new uint8_t[FF_MAX_SS];
    MKFS_PARM parameters{
        FM_ANY | FM_SFD,
        0,
        0,
        0,
        0
    };

    auto result = f_mkfs(static_cast<const char*>(Util::Memory::String::format("%u", volumeId)), &parameters, work, FF_MAX_SS);
    volumeIdAllocator.unset(volumeId);
    return result == FR_OK;
}

Node* FatDriver::getNode(const Util::Memory::String &path) {
    auto fatPath = Util::Memory::String::format("%u:%s", volumeId, static_cast<const char*>(path));
    return FatNode::open(fatPath);
}

bool FatDriver::createNode(const Util::Memory::String &path, Util::File::Type type) {
    auto fatPath = Util::Memory::String::format("%u:%s", volumeId, static_cast<const char*>(path));
    FRESULT result = FR_DISK_ERR;

    if (type == Util::File::DIRECTORY) {
        result = f_mkdir(static_cast<const char*>(fatPath));
    } else if (type == Util::File::REGULAR) {
        FIL *file = new FIL;
        result = f_open(file, static_cast<const char*>(fatPath), FA_CREATE_NEW);
        if (result == FR_OK) {
            f_close(file);
        }

        delete file;
    }

    return result == FR_OK;
}

bool FatDriver::deleteNode(const Util::Memory::String &path) {
    auto fatPath = Util::Memory::String::format("%u:%s", volumeId, static_cast<const char*>(path));
    auto result = f_unlink(static_cast<const char*>(fatPath));

    return result == FR_OK;
}

}