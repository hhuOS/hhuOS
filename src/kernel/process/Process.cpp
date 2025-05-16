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


#include "Process.h"

#include "kernel/memory/VirtualAddressSpace.h"
#include "kernel/process/Thread.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/async/IdGenerator.h"
#include "lib/util/collection/Iterator.h"
#include "kernel/service/Service.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Scheduler.h"

namespace Kernel {
class FileDescriptorManager;

Util::Async::IdGenerator Process::idGenerator;

Process::Process(VirtualAddressSpace &addressSpace, const Util::String &name, const Util::Io::File &workingDirectory) :
        id(idGenerator.next()), name(name), addressSpace(addressSpace), workingDirectory(workingDirectory) {}

Process::~Process() {
    Kernel::Service::getService<Kernel::MemoryService>().removeAddressSpace(addressSpace);
}

bool Process::operator==(const Process &other) const {
    return id == other.id;
}

uint32_t Process::getId() const {
    return id;
}

VirtualAddressSpace &Process::getAddressSpace() {
    return addressSpace;
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

bool Process::setWorkingDirectory(const Util::String &path) {
    auto file = getFileFromPath(path);
    if (!file.exists() || file.isFile()) {
        return false;
    }

    workingDirectory = file;
    return true;
}

Util::Io::File Process::getWorkingDirectory() {
    return workingDirectory;
}

Util::Io::File Process::getFileFromPath(const Util::String &path) {
    if (path[0] == '/') {
        return Util::Io::File(path);
    }

    return Util::Io::File(workingDirectory.getCanonicalPath() + "/" + path);
}

void Process::setExitCode(int32_t code) {
    exitCode = code;
    finished = true;
}

bool Process::isKernelProcess() const {
    return addressSpace.isKernelAddressSpace();
}

uint32_t Process::getThreadCount() const {
    return threads.size();
}

void Process::setMainThread(Thread &thread) {
    mainThread = &thread;
}

void Process::join() {
    auto &scheduler = Service::getService<ProcessService>().getScheduler();
    while (mainThread == nullptr) {
        if (finished) {
            return;
        }

        scheduler.yield();
    }

    mainThread->join();
}

Util::String Process::getName() const {
    return name;
}

Util::Array<Thread*> Process::getThreads() const {
    return threads.toArray();
}

void Process::addThread(Thread &thread) {
    threads.add(&thread);
}

void Process::removeThread(Thread &thread) {
    threads.remove(&thread);
}

void Process::killAllThreadsButCurrent() {
    auto &scheduler = Service::getService<ProcessService>().getScheduler();
    auto currentThreadId = scheduler.getCurrentThread().getId();

    for (auto *thread : threads) {
        if (thread->getId() != currentThreadId) {
            scheduler.kill(*thread);
        }
    }
}

}