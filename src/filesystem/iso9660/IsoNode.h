/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The ISO9660 driver is based on a bachelor's thesis, written by Moritz Riefer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-morie103
 */

#ifndef HHUOS_ISONODE_H
#define HHUOS_ISONODE_H

#include <stdint.h>

#include "filesystem/Node.h"
#include "IsoDriver.h"
#include "lib/util/io/file/File.h"

namespace Device {
namespace Storage {
class StorageDevice;
}  // namespace Storage
}  // namespace Device

namespace Filesystem::Iso {

class IsoNode : public Node {

public:
    /**
     * Constructor.
     */
    explicit IsoNode(Device::Storage::StorageDevice &device, const IsoDriver::DirectoryRecord *record);

    /**
     * Copy Constructor.
     */
    IsoNode(const IsoNode &other) = delete;

    /**
     * Assignment operator.
     */
    IsoNode &operator=(const IsoNode &other) = delete;

    /**
     * Destructor.
     */
    ~IsoNode() override;

    /**
     * Overriding function from Node.
     */
    Util::String getName() override;

    /**
     * Overriding function from Node.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from Node.
     */
    Util::Io::File::Type getType() override;

    /**
     * Overriding function from Node.
     */
    Util::Array<Util::String> getChildren() override;

    /**
     * Overriding function from Node.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from Node.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    Device::Storage::StorageDevice &device;
    const IsoDriver::DirectoryRecord &record;
};

}

#endif
