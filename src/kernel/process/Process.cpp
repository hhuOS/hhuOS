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

#include "kernel/system/System.h"
#include "Process.h"
#include "ProcessScheduler.h"

namespace Kernel {

Util::Async::IdGenerator<uint32_t> Process::idGenerator;

Process::Process(ProcessScheduler &scheduler, VirtualAddressSpace &addressSpace, const Util::File::File &workingDirectory) :
        id(idGenerator.next()), addressSpace(addressSpace), scheduler(scheduler), threadScheduler(scheduler), workingDirectory(workingDirectory) {}

Process::~Process() {
    Kernel::System::getService<Kernel::MemoryService>().removeAddressSpace(addressSpace);
}

void Process::ready(Thread &thread) {
    threadScheduler.ready(thread);
}

void Process::start() {
    scheduler.ready(*this);
}

void Process::exit(int32_t code) {
    exitCode = code;
    finished = true;

    scheduler.exit();
}

uint32_t Process::getId() const {
    return id;
}

VirtualAddressSpace &Process::getAddressSpace() {
    return addressSpace;
}

ThreadScheduler &Process::getThreadScheduler() {
    return threadScheduler;
}

FileDescriptorManager &Process::getFileDescriptorManager() {
    return fileDescriptorManager;
}

bool Process::isFinished() const {
    return finished;
}

int32_t Process::getExitCode() const {
    return exitCode;
}

bool Process::setWorkingDirectory(const Util::Memory::String &path) {
    auto file = getFileFromPath(path);
    if (!file.exists() || file.isFile()) {
        return false;
    }

    workingDirectory = file;
    return true;
}

Util::File::File Process::getWorkingDirectory() {
    return workingDirectory;
}

Util::File::File Process::getFileFromPath(const Util::Memory::String &path) {
    if (path[0] == '/') {
        return Util::File::File(path);
    }

    return Util::File::File(workingDirectory.getCanonicalPath() + "/" + path);
}

}