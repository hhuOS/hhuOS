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
#include "lib/util/base/Exception.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/System.h"
#include "lib/util/collection/Iterator.h"

namespace Kernel {
class VirtualAddressSpace;

ProcessService::ProcessService() : kernelProcess(createProcess(System::getService<MemoryService>().getKernelAddressSpace(), "Kernel", Util::Io::File("/"), Util::Io::File("/device/terminal"), Util::Io::File("/device/terminal"), Util::Io::File("/device/terminal"))) {
    SystemCall::registerSystemCall(Util::System::EXIT_PROCESS, [](uint32_t paramCount, va_list arguments) -> bool {
        auto &processService = System::getService<ProcessService>();
        int32_t exitCode = paramCount >=1 ? va_arg(arguments, int32_t) : 0;

        processService.exitCurrentProcess(exitCode);
    });

    SystemCall::registerSystemCall(Util::System::EXECUTE_BINARY, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 7) {
            return false;
        }

        auto &processService = System::getService<ProcessService>();
        auto *binaryFile = va_arg(arguments, Util::Io::File*);
        auto *inputFile = va_arg(arguments, Util::Io::File*);
        auto *outputFile = va_arg(arguments, Util::Io::File*);
        auto *errorFile = va_arg(arguments, Util::Io::File*);
        auto *command = va_arg(arguments, const Util::String*);
        auto *commandArguments = va_arg(arguments, Util::Array<Util::String>*);
        auto &processId = *va_arg(arguments, uint32_t*);

        auto &process = processService.loadBinary(*binaryFile, *inputFile, *outputFile, *errorFile, *command, *commandArguments);

        processId = process.getId();
        return true;
    });

    SystemCall::registerSystemCall(Util::System::GET_CURRENT_PROCESS, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &processService = System::getService<ProcessService>();
        auto &processId = *va_arg(arguments, uint32_t*);

        processId = processService.getCurrentProcess().getId();
        return true;
    });

    SystemCall::registerSystemCall(Util::System::JOIN_PROCESS, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &processService = System::getService<ProcessService>();
        auto processId = va_arg(arguments, uint32_t);

        auto *process = processService.getProcess(processId);
        if (process == nullptr) {
            return false;
        }

        process->join();
        return true;
    });

    SystemCall::registerSystemCall(Util::System::KILL_PROCESS, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &processService = System::getService<ProcessService>();
        auto processId = va_arg(arguments, int32_t);

        auto *process = processService.getProcess(processId);
        if (process == nullptr) {
            return false;
        }

        processService.killProcess(*process);
        return true;
    });
}

Process& ProcessService::createProcess(VirtualAddressSpace &addressSpace, const Util::String &name, const Util::Io::File &workingDirectory, const Util::Io::File &standardIn, const Util::Io::File &standardOut, const Util::Io::File &standardError) {
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

Process& ProcessService::loadBinary(const Util::Io::File &binaryFile, const Util::Io::File &inputFile, const Util::Io::File &outputFile, const Util::Io::File &errorFile, const Util::String &command, const Util::Array<Util::String> &arguments) {
    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();

    auto &virtualAddressSpace = memoryService.createAddressSpace();
    auto &process = createProcess(virtualAddressSpace, binaryFile.getCanonicalPath(), Util::Io::File::getCurrentWorkingDirectory(), inputFile, outputFile, errorFile);
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

Util::Array<uint32_t> ProcessService::getActiveProcessIds() const {
    auto ids = Util::Array<uint32_t>(processList.size());
    for (uint32_t i = 0; i < processList.size(); i++) {
        ids[i] = processList.get(i)->getId();
    }

    return ids;
}

}