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
#include "kernel/paging/VirtualAddressSpace.h"
#include "kernel/process/Thread.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/async/IdGenerator.h"

namespace Kernel {

Util::Async::IdGenerator<uint32_t> Process::idGenerator;

Process::Process(VirtualAddressSpace &addressSpace, const Util::Memory::String &name, const Util::File::File &workingDirectory) :
        id(idGenerator.next()), name(name), addressSpace(addressSpace), workingDirectory(workingDirectory) {}

Process::~Process() {
    Kernel::System::getService<Kernel::MemoryService>().removeAddressSpace(addressSpace);
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
    auto &schedulerService = System::getService<Kernel::SchedulerService>();
    while (mainThread == nullptr) {
        if (finished) {
            return;
        }

        schedulerService.yield();
    }

    mainThread->join();
}

Util::Memory::String Process::getName() const {
    return name;
}

Util::Data::Array<Thread*> Process::getThreads() const {
    return threads.toArray();
}

void Process::addThread(Thread &thread) {
    threads.add(&thread);
}

void Process::removeThread(Thread &thread) {
    threads.remove(&thread);
}

void Process::killAllThreadsButCurrent() {
    auto &schedulerService = System::getService<SchedulerService>();
    auto currentThreadId = schedulerService.getCurrentThread().getId();

    for (auto *thread : threads) {
        if (thread->getId() != currentThreadId) {
            schedulerService.killWithoutLock(*thread);
        }
    }
}

}