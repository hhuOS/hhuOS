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

#ifndef HHUOS_FATDRIVER_H
#define HHUOS_FATDRIVER_H

#include <stdint.h>

#include "filesystem/fat/ff/source/ff.h"
#include "filesystem/PhysicalDriver.h"
#include "lib/util/base/String.h"
#include "lib/util/reflection/Prototype.h"
#include "lib/util/io/file/File.h"

namespace Device {
namespace Storage {
class StorageDevice;
}  // namespace Storage
}  // namespace Device
namespace Util {

template <typename T> class Array;

namespace Async {
class AtomicBitmap;
}  // namespace Async

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
    Node* getNode(const Util::String &path) override;

    /**
     * Overriding function from Driver.
     */
    bool createNode(const Util::String &path, Util::Io::File::Type type) override;

    /**
     * Overriding function from Driver.
     */
    bool deleteNode(const Util::String &path) override;

    static Device::Storage::StorageDevice& getStorageDevice(uint8_t volumeId);

private:

    uint32_t volumeId{};
    FATFS fatVolume{};

    static Util::Async::AtomicBitmap volumeIdAllocator;
    static Util::Array<Device::Storage::StorageDevice*> deviceMap;
};

}

#endif
