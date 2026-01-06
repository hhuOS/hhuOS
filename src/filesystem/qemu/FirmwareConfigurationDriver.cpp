/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "FirmwareConfigurationDriver.h"

#include <stdint.h>

#include "FirmwareConfigurationNode.h"
#include "device/system/FirmwareConfiguration.h"
#include "filesystem/memory/MemoryDirectoryNode.h"
#include "lib/util/collection/Array.h"
#include "lib/util/base/String.h"

namespace Filesystem::Qemu {

FirmwareConfigurationDriver::FirmwareConfigurationDriver(Device::FirmwareConfiguration &device) {
    for (const auto &file : device.getFiles()) {
        auto tokens = Util::String(file.name).split("/");

        Util::String path = "/";
        for (uint32_t i = 0; i < tokens.length() - 1; i++) {
            addNode(path, new Memory::MemoryDirectoryNode(tokens[i]));
            path += tokens[i] + "/";
        }

        addNode(path, new FirmwareConfigurationNode(tokens[tokens.length() - 1], file, device));
    }
}

bool FirmwareConfigurationDriver::createNode([[maybe_unused]] const Util::String &path, [[maybe_unused]] Util::Io::File::Type type) {
    return false;
}

bool FirmwareConfigurationDriver::deleteNode([[maybe_unused]] const Util::String &path) {
    return false;
}

}