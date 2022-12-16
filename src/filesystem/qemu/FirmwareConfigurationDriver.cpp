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

#include "FirmwareConfigurationDriver.h"

#include <cstdint>

#include "FirmwareConfigurationNode.h"
#include "device/debug/FirmwareConfiguration.h"
#include "filesystem/memory/MemoryDirectoryNode.h"
#include "lib/util/data/Array.h"

namespace Filesystem::Qemu {

FirmwareConfigurationDriver::FirmwareConfigurationDriver(Device::FirmwareConfiguration &device) {
    for (const auto &file : device.getFiles()) {
        auto tokens = Util::Memory::String(file.name).split("/");

        Util::Memory::String path = "/";
        for (uint32_t i = 0; i < tokens.length() - 1; i++) {
            addNode(path, new Memory::MemoryDirectoryNode(tokens[i]));
            path += tokens[i] + "/";
        }

        addNode(path, new FirmwareConfigurationNode(tokens[tokens.length() - 1], file, device));
    }
}

bool FirmwareConfigurationDriver::createNode(const Util::Memory::String &path, Util::File::Type type) {
    return false;
}

bool FirmwareConfigurationDriver::deleteNode(const Util::Memory::String &path) {
    return false;
}

}