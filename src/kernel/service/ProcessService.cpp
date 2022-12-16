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

#include <stdarg.h>

#include "kernel/process/AddressSpaceCleaner.h"
#include "kernel/system/System.h"
#include "kernel/process/BinaryLoader.h"
#include "ProcessService.h"
#include "FilesystemService.h"
#include "kernel/file/FileDescriptorManager.h"
#include "kernel/process/Process.h"
#include "kernel/process/Thread.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/SchedulerService.h"
#include "kernel/system/SystemCall.h"
#include "lib/util/Exception.h"
#include "lib/util/file/File.h"
#include "lib/util/system/System.h"

namespace Kernel {
class VirtualAddressSpace;

ProcessService::ProcessService() : kernelProcess(createProcess(System::getService<MemoryService>().getKernelAddressSpace(), "Kernel", Util::File::File("/"), Util::File::File("/device/terminal"), Util::File::File("/device/terminal"), Util::File::File("/device/terminal"))) {
    SystemCall::registerSystemCall(Util::System::EXIT_PROCESS, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        int32_t exitCode = 0;
        if (paramCount >= 1) {
            exitCode = va_arg(arguments, int32_t);
        }

        auto &processService = System::getService<ProcessService>();
        processService.exitCurrentProcess(exitCode);
    });

    SystemCall::registerSystemCall(Util::System::EXECUTE_BINARY, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 7) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto *binaryFile = va_arg(arguments, Util::File::File*);
        auto *inputFile = va_arg(arguments, Util::File::File*);
        auto *outputFile = va_arg(arguments, Util::File::File*);
        auto *errorFile = va_arg(arguments, Util::File::File*);
        auto *command = va_arg(arguments, const Util::Memory::String*);
        auto *commandArguments = va_arg(arguments, Util::Data::Array<Util::Memory::String>*);
        auto *processId = va_arg(arguments, uint32_t*);

        auto &processService = System::getService<ProcessService>();
        auto &process = processService.loadBinary(*binaryFile, *inputFile, *outputFile, *errorFile, *command, *commandArguments);

        *processId = process.getId();
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::GET_CURRENT_PROCESS, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto *processId = va_arg(arguments, uint32_t*);

        auto &processService = System::getService<ProcessService>();
        *processId = processService.getCurrentProcess().getId();

        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::JOIN_PROCESS, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto processId = va_arg(arguments, uint32_t);

        auto &processService = System::getService<ProcessService>();
        auto *process = processService.getProcess(processId);

        if (process == nullptr) {
            return Util::System::INVALID_ARGUMENT;
        }

        process->join();
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::KILL_PROCESS, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        int32_t processId = va_arg(arguments, int32_t);

        auto &processService = System::getService<ProcessService>();
        auto *process = processService.getProcess(processId);
        if (process == nullptr) {
            return Util::System::INVALID_ARGUMENT;
        }

        processService.killProcess(*process);
        return Util::System::OK;
    });
}

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

void ProcessService::killProcess(Process &process) {
    if (process == getCurrentProcess()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "A process cannot kill itself!");
    }

    auto &schedulerService = System::getService<SchedulerService>();
    schedulerService.lockScheduler();
    for (auto *thread : process.getThreads()) {
        schedulerService.killWithoutLock(*thread);
    }
    schedulerService.unlockScheduler();

    auto &cleanerThread = Thread::createKernelThread("Address-Space-Cleaner", process, new AddressSpaceCleaner());
    schedulerService.ready(cleanerThread);
    process.setExitCode(-1);

    lock.acquire();
    processList.remove(&process);
    lock.release();
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