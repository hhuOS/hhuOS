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

#include "FirmwareConfigurationNode.h"

namespace Filesystem::Qemu {

FirmwareConfigurationNode::FirmwareConfigurationNode(const Util::String &name, const Device::FirmwareConfiguration::File &file,
     Device::FirmwareConfiguration &device) : Memory::MemoryNode(name), file(file), device(device) {}

uint64_t FirmwareConfigurationNode::getLength() {
    return file.size;
}

uint64_t FirmwareConfigurationNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    return device.readFile(file, targetBuffer, pos, numBytes);
}

uint64_t FirmwareConfigurationNode::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    return device.writeFile(file, sourceBuffer, pos, numBytes);
}

}