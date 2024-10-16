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

#include "ProcessDriver.h"

#include "kernel/service/ProcessService.h"
#include "ProcessDirectoryNode.h"
#include "ProcessRootNode.h"
#include "ProcessFileNode.h"
#include "kernel/process/Process.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "kernel/service/Service.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Filesystem::Process {

Node* ProcessDriver::getNode(const Util::String &path) {
    if (path.isEmpty() || path == "/") {
        return new ProcessRootNode();
    }

    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    auto ids = processService.getActiveProcessIds();
    auto splitPath = path.split(Util::Io::File::SEPARATOR);
    auto id = Util::String::parseInt(splitPath[0]);

    auto *process = processService.getProcess(id);
    if (process == nullptr) {
        return nullptr;
    }

    if (splitPath.length() == 1) {
        return new ProcessDirectoryNode(id);
    } else if (splitPath.length() == 2) {
        const auto &name = splitPath[1];
        if (name == "name") {
            return new ProcessFileNode(name, process->getName());
        } else if (name == "cwd") {
            return new ProcessFileNode(name, process->getWorkingDirectory().getCanonicalPath());
        } else if (name == "thread_count") {
            return new ProcessFileNode(name, Util::String::format("%u", process->getThreadCount()));
        }
    }

    return nullptr;
}

bool ProcessDriver::createNode([[maybe_unused]] const Util::String &path, [[maybe_unused]] Util::Io::File::Type type) {
    return false;
}

bool ProcessDriver::deleteNode([[maybe_unused]] const Util::String &path) {
    return false;
}

}