/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "ProcessRootNode.h"

#include "kernel/service/ProcessService.h"
#include "kernel/service/Service.h"


namespace Filesystem::Process {

Util::String ProcessRootNode::getName() {
    return "";
}

Util::Io::File::Type ProcessRootNode::getType() {
    return Util::Io::File::DIRECTORY;
}

uint64_t ProcessRootNode::getLength() {
    return 0;
}

Util::Array<Util::String> ProcessRootNode::getChildren() {
    auto ids = Kernel::Service::getService<Kernel::ProcessService>().getActiveProcessIds();
    auto ret = Util::Array<Util::String>(ids.length());

    for (uint32_t i = 0; i < ids.length(); i++) {
        ret[i] = Util::String::format("%u", ids[i]);
    }

    return ret;
}

uint64_t ProcessRootNode::readData([[maybe_unused]] uint8_t *targetBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    return 0;
}

uint64_t ProcessRootNode::writeData([[maybe_unused]] const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    return 0;
}

}