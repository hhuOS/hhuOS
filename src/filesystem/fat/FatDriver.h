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

#ifndef HHUOS_FATDRIVER_H
#define HHUOS_FATDRIVER_H

#include <cstdint>

#include "filesystem/fat/ff/source/ff.h"
#include "filesystem/core/PhysicalDriver.h"
#include "lib/util/file/Type.h"
#include "lib/util/memory/String.h"
#include "lib/util/reflection/Prototype.h"

namespace Device {
namespace Storage {
class StorageDevice;
}  // namespace Storage
}  // namespace Device
namespace Filesystem {
class Node;
}  // namespace Filesystem
namespace Util {
namespace Data {
template <typename T> class Array;
}  // namespace Data
namespace Memory {
class AtomicBitmap;
}  // namespace Memory
}  // namespace Util

namespace Filesystem::Fat {

class FatDriver : public PhysicalDriver {

public:
    /**
     * Constructor.
     */
    FatDriver() = default;

    /**
     * Copy Constructor.
     */
    FatDriver(const FatDriver &copy) = delete;

    /**
     * Assignment operator.
     */
    FatDriver& operator=(const FatDriver &other) = delete;

    /**
     * Destructor.
     */
    ~FatDriver() override;

    PROTOTYPE_IMPLEMENT_CLONE(FatDriver);

    PROTOTYPE_IMPLEMENT_GET_CLASS_NAME("Filesystem::Fat::FatDriver")

    /**
     * Overriding function from Driver.
     */
    bool mount(Device::Storage::StorageDevice &device) override;

    /**
     * Overriding function from Driver.
     */
    bool createFilesystem(Device::Storage::StorageDevice &device) override;

    /**
     * Overriding function from Driver.
     */
    Node* getNode(const Util::Memory::String &path) override;

    /**
     * Overriding function from Driver.
     */
    bool createNode(const Util::Memory::String &path, Util::File::Type type) override;

    /**
     * Overriding function from Driver.
     */
    bool deleteNode(const Util::Memory::String &path) override;

    static Device::Storage::StorageDevice& getStorageDevice(uint8_t volumeId);

private:

    uint32_t volumeId{};
    FATFS fatVolume{};

    static Util::Memory::AtomicBitmap volumeIdAllocator;
    static Util::Data::Array<Device::Storage::StorageDevice*> deviceMap;
};

}

#endif
