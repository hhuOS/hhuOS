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

#include "kernel/process/AddressSpaceCleaner.h"
#include "kernel/system/System.h"
#include "kernel/process/BinaryLoader.h"
#include "ProcessService.h"
#include "FilesystemService.h"

namespace Kernel {

ProcessService::ProcessService() : kernelProcess(createProcess(System::getService<MemoryService>().getKernelAddressSpace(), "Kernel", Util::File::File("/"), Util::File::File("/device/terminal"), Util::File::File("/device/terminal"), Util::File::File("/device/terminal"))) {}

Process& ProcessService::createProcess(VirtualAddressSpace &addressSpace, const Util::Memory::String &name, const Util::File::File &workingDirectory, const Util::File::File &standardIn, const Util::File::File &standardOut, const Util::File::File &standardError) {
    auto *process = new Process(addressSpace, name, workingDirectory);

    // Create standard file descriptors
    if (System::isServiceRegistered(FilesystemService::SERVICE_ID)) {
        process->getFileDescriptorManager().openFile(standardIn.getCanonicalPath());
        process->getFileDescriptorManager().openFile(standardOut.getCanonicalPath());
        process->getFileDescriptorManager().openFile(standardError.getCanonicalPath());
    }

    lock.acquire();
    processList.add(process);
    lock.release();

    return *process;
}

Process& ProcessService::loadBinary(const Util::File::File &binaryFile, const Util::File::File &inputFile, const Util::File::File &outputFile, const Util::File::File &errorFile, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments) {
    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();

    auto &virtualAddressSpace = memoryService.createAddressSpace();
    auto &process = createProcess(virtualAddressSpace, binaryFile.getCanonicalPath(), Util::File::getCurrentWorkingDirectory(), inputFile, outputFile, errorFile);
    auto &thread = Kernel::Thread::createKernelThread("Loader", process, new Kernel::BinaryLoader(binaryFile.getCanonicalPath(), command, arguments));

    System::getService<SchedulerService>().ready(thread);
    return process;
}

Process& ProcessService::getCurrentProcess() {
    return System::getService<SchedulerService>().getCurrentThread().getParent();
}

bool ProcessService::isProcessActive(uint32_t id) {
    for (const auto *process : processList) {
        if (process->getId() == id) {
            return true;
        }
    }

    return false;
}

void ProcessService::exitCurrentProcess(int32_t exitCode) {
    auto &schedulerService = System::getService<SchedulerService>();
    auto &process = getCurrentProcess();
    auto &cleanerThread = Thread::createKernelThread("Address-Space-Cleaner", process, new AddressSpaceCleaner());

    process.killAllThreadsButCurrent();
    schedulerService.ready(cleanerThread);

    process.setExitCode(exitCode);

    lock.acquire();
    processList.remove(&process);
    lock.release();

    schedulerService.exitCurrentThread();

    __builtin_unreachable();
}

Process* ProcessService::getProcess(uint32_t id) {
    for (auto *process : processList) {
        if (process->getId() == id) {
            return process;
        }
    }

    return nullptr;
}

Process& ProcessService::getKernelProcess() const {
    return kernelProcess;
}

Util::Data::Array<uint32_t> ProcessService::getActiveProcessIds() const {
    auto ids = Util::Data::Array<uint32_t>(processList.size());
    for (uint32_t i = 0; i < processList.size(); i++) {
        ids[i] = processList.get(i)->getId();
    }

    return ids;
}

}