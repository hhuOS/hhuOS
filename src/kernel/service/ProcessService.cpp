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

#include <stdarg.h>

#include "kernel/process/AddressSpaceCleaner.h"
#include "kernel/process/BinaryLoader.h"
#include "ProcessService.h"
#include "FilesystemService.h"
#include "kernel/process/FileDescriptorManager.h"
#include "kernel/process/Process.h"
#include "kernel/process/Thread.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/base/Exception.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/System.h"
#include "lib/util/collection/Iterator.h"
#include "InterruptService.h"
#include "kernel/service/Service.h"
#include "kernel/process/SchedulerCleaner.h"

namespace Util {
namespace Async {
class Runnable;
}  // namespace Async
namespace Time {
class Timestamp;
}  // namespace Time
}  // namespace Util


namespace Kernel {
class VirtualAddressSpace;

ProcessService::ProcessService(Process *kernelProcess) : kernelProcess(kernelProcess) {
    processList.add(kernelProcess);

    Service::getService<InterruptService>().assignSystemCall(Util::System::YIELD, [](uint32_t, va_list) -> bool {
        Service::getService<ProcessService>().getScheduler().yield();
        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::GET_CURRENT_THREAD, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &processService = Service::getService<ProcessService>();
        auto &threadId = *va_arg(arguments, uint32_t*);

        threadId = processService.getScheduler().getCurrentThread().getId();
        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::CREATE_THREAD, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 4) {
            return false;
        }

        auto &processService = Service::getService<ProcessService>();
        auto *name = va_arg(arguments, const char*);
        auto *runnable = va_arg(arguments, Util::Async::Runnable*);
        auto eip = va_arg(arguments, uint32_t);
        auto &threadId = *va_arg(arguments, uint32_t*);

        auto &thread = Kernel::Thread::createUserThread(name, processService.getCurrentProcess(), eip, runnable);

        threadId = thread.getId();
        processService.getScheduler().ready(thread);
        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::SLEEP, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &processService = Service::getService<ProcessService>();
        auto &time = *va_arg(arguments, Util::Time::Timestamp*);

        processService.getScheduler().sleep(time);
        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::JOIN_THREAD, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &processService = Service::getService<ProcessService>();
        auto threadId = va_arg(arguments, uint32_t);

        auto *thread = processService.getScheduler().getThread(threadId);
        if (thread != nullptr) {
            thread->join();
        }

        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::EXIT_THREAD, []([[maybe_unused]] uint32_t paramCount, [[maybe_unused]] va_list arguments) -> bool {
        Service::getService<ProcessService>().getScheduler().exit();
        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::EXIT_PROCESS, [](uint32_t paramCount, va_list arguments) -> bool {
        auto &processService = Service::getService<ProcessService>();
        int32_t exitCode = paramCount >= 1 ? va_arg(arguments, int32_t) : 0;

        processService.exitCurrentProcess(exitCode);
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::EXECUTE_BINARY, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 7) {
            return false;
        }

        auto &processService = Service::getService<ProcessService>();
        auto *binaryFile = va_arg(arguments, Util::Io::File*);
        auto *inputFile = va_arg(arguments, Util::Io::File*);
        auto *outputFile = va_arg(arguments, Util::Io::File*);
        auto *errorFile = va_arg(arguments, Util::Io::File*);
        auto *command = va_arg(arguments, const Util::String*);
        auto *commandArguments = va_arg(arguments, Util::Array<Util::String>*);
        auto &processId = *va_arg(arguments, uint32_t*);

        auto &process = processService.loadBinary(*binaryFile, *inputFile, *outputFile, *errorFile, *command,*commandArguments);

        processId = process.getId();
        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::GET_CURRENT_PROCESS, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &processService = Service::getService<ProcessService>();
        auto &processId = *va_arg(arguments, uint32_t*);

        processId = processService.getCurrentProcess().getId();
        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::JOIN_PROCESS, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &processService = Service::getService<ProcessService>();
        auto processId = va_arg(arguments, uint32_t);

        auto *process = processService.getProcess(processId);
        if (process == nullptr) {
            return false;
        }

        process->join();
        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::KILL_PROCESS, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &processService = Service::getService<ProcessService>();
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
    if (Service::isServiceRegistered(FilesystemService::SERVICE_ID)) {
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
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();

    auto &virtualAddressSpace = memoryService.createAddressSpace();
    auto &process = createProcess(virtualAddressSpace, binaryFile.getCanonicalPath(), Util::Io::File::getCurrentWorkingDirectory(), inputFile, outputFile, errorFile);
    auto &thread = Kernel::Thread::createKernelThread("Loader", process, new Kernel::BinaryLoader(binaryFile.getCanonicalPath(), command, arguments));

    scheduler.ready(thread);
    return process;
}

void ProcessService::killProcess(Process &process) {
    if (process == getCurrentProcess()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "A process cannot kill itself!");
    }

    for (auto *thread : process.getThreads()) {
        scheduler.kill(*thread);
    }

    auto &cleanerThread = Thread::createKernelThread("Address-Space-Cleaner", process, new AddressSpaceCleaner());
    scheduler.ready(cleanerThread);
    process.setExitCode(-1);

    lock.acquire();
    processList.remove(&process);
    lock.release();
}

Process& ProcessService::getCurrentProcess() {
    if (!scheduler.isInitialized()) {
        return *kernelProcess;
    }

    return scheduler.getCurrentThread().getParent();
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
    auto &process = getCurrentProcess();
    auto &cleanerThread = Thread::createKernelThread("Address-Space-Cleaner", process, new AddressSpaceCleaner());

    process.killAllThreadsButCurrent();
    scheduler.ready(cleanerThread);

    process.setExitCode(exitCode);

    lock.acquire();
    processList.remove(&process);
    lock.release();

    scheduler.exit();

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
    return *kernelProcess;
}

Util::Array<uint32_t> ProcessService::getActiveProcessIds() const {
    auto ids = Util::Array<uint32_t>(processList.size());
    for (uint32_t i = 0; i < processList.size(); i++) {
        ids[i] = processList.get(i)->getId();
    }

    return ids;
}

Scheduler &ProcessService::getScheduler() {
    return scheduler;
}

void ProcessService::cleanup(Thread *thread) {
    cleaner->cleanup(thread);
}

void ProcessService::cleanup(Process *process) {
    cleaner->cleanup(process);
}

void ProcessService::startScheduler() {
    cleaner = new Kernel::SchedulerCleaner();
    auto &schedulerCleanerThread = Kernel::Thread::createKernelThread("Scheduler-Cleaner", *kernelProcess, cleaner);
    scheduler.ready(schedulerCleanerThread);

    scheduler.start();
}

}