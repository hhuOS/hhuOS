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

#ifndef HHUOS_FIRMWARECONFIGURATIONNODE_H
#define HHUOS_FIRMWARECONFIGURATIONNODE_H

#include <cstdint>

#include "filesystem/memory/MemoryNode.h"
#include "device/debug/FirmwareConfiguration.h"
#include "lib/util/memory/String.h"

namespace Filesystem::Qemu {

class FirmwareConfigurationNode : public Memory::MemoryNode {

public:
    /**
     * Constructor.
     */
    explicit FirmwareConfigurationNode(const Util::Memory::String &name, const Device::FirmwareConfiguration::File &file, Device::FirmwareConfiguration &device);

    /**
     * Copy Constructor.
     */
    FirmwareConfigurationNode(const FirmwareConfigurationNode &other) = delete;

    /**
     * Assignment operator.
     */
    FirmwareConfigurationNode &operator=(const FirmwareConfigurationNode &other) = delete;

    /**
     * Destructor.
     */
    ~FirmwareConfigurationNode() override = default;

    /**
     * Overriding function from Node.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from Node.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from Node.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    Device::FirmwareConfiguration::File file{};
    Device::FirmwareConfiguration &device;
};

}

#endif
