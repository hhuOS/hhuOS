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
#include "kernel/process/AddressSpaceCleaner.h"
#include "kernel/process/BinaryLoader.h"
#include "device/cpu/Fpu.h"
#include "FilesystemService.h"
#include "SchedulerService.h"

namespace Kernel {

Logger SchedulerService::log = Logger::get("Scheduler");

SchedulerService::SchedulerService() : kernelProcess(createProcess(System::getService<MemoryService>().getKernelAddressSpace(), "Kernel", Util::File::File("/"), Util::File::File("/device/terminal"), Util::File::File("/device/terminal"), Util::File::File("/device/terminal"))) {
    defaultFpuContext = static_cast<uint8_t*>(System::getService<MemoryService>().allocateKernelMemory(512, 16));
    Util::Memory::Address<uint32_t>(defaultFpuContext).setRange(0, 512);

    if (Device::Fpu::isAvailable()) {
        log.info("FPU detected -> Enabling FPU context switching");
        fpu = new Device::Fpu(defaultFpuContext);
        fpu->plugin();
    } else {
        log.warn("No FPU present");
    }

    SystemCall::registerSystemCall(Util::System::YIELD, [](uint32_t, va_list) -> Util::System::Result {
        System::getService<SchedulerService>().yield();
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::EXIT_PROCESS, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        int32_t exitCode = 0;
        if (paramCount >= 1) {
            exitCode = va_arg(arguments, int32_t);
        }

        auto &schedulerService = System::getService<SchedulerService>();
        schedulerService.exitCurrentProcess(exitCode);
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

        auto &schedulerService = System::getService<SchedulerService>();
        auto &process = schedulerService.loadBinary(*binaryFile, *inputFile, *outputFile, *errorFile, *command, *commandArguments);

        *processId = process.getId();
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::GET_CURRENT_PROCESS, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto *processId = va_arg(arguments, uint32_t*);

        auto &schedulerService = System::getService<SchedulerService>();
        *processId = schedulerService.getCurrentProcess().getId();

        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::GET_CURRENT_THREAD, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto *threadId = va_arg(arguments, uint32_t*);

        auto &schedulerService = System::getService<SchedulerService>();
        *threadId = schedulerService.getCurrentThread().getId();

        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::JOIN_PROCESS, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto processId = va_arg(arguments, uint32_t);

        auto &schedulerService = System::getService<SchedulerService>();
        auto *process = schedulerService.getProcess(processId);

        if (process == nullptr) {
            return Util::System::INVALID_ARGUMENT;
        }

        process->join();
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::SLEEP, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto *time = va_arg(arguments, Util::Time::Timestamp*);

        System::getService<SchedulerService>().sleep(*time);
        return Util::System::Result::OK;
    });
}

void SchedulerService::kickoffThread() {
    scheduler.getCurrentThread().run();
    scheduler.exit();
}

void SchedulerService::startScheduler() {
    cleaner = new Kernel::SchedulerCleaner();
    auto &schedulerCleanerThread = Kernel::Thread::createKernelThread("Scheduler-Cleaner", kernelProcess, cleaner);
    ready(schedulerCleanerThread);
    
    scheduler.start();
}

void SchedulerService::ready(Thread &thread) {
    scheduler.ready(thread);
}

Process& SchedulerService::createProcess(VirtualAddressSpace &addressSpace, const Util::Memory::String &name, const Util::File::File &workingDirectory, const Util::File::File &standardIn, const Util::File::File &standardOut, const Util::File::File &standardError) {
    auto *process = new Process(addressSpace, name, workingDirectory);

    // Create standard file descriptors
    if (System::isServiceRegistered(FilesystemService::SERVICE_ID)) {
        process->getFileDescriptorManager().openFile(standardIn.getCanonicalPath());
        process->getFileDescriptorManager().openFile(standardOut.getCanonicalPath());
        process->getFileDescriptorManager().openFile(standardError.getCanonicalPath());
    }

    processLock.acquire();
    processList.add(process);
    processLock.release();

    return *process;
}

Process &SchedulerService::loadBinary(const Util::File::File &binaryFile, const Util::File::File &inputFile, const Util::File::File &outputFile, const Util::File::File &errorFile, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments) {
    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();

    auto &virtualAddressSpace = memoryService.createAddressSpace();
    auto &process = createProcess(virtualAddressSpace, binaryFile.getCanonicalPath(), Util::File::getCurrentWorkingDirectory(), inputFile, outputFile, errorFile);
    auto &thread = Kernel::Thread::createKernelThread("Loader", process, new Kernel::BinaryLoader(binaryFile.getCanonicalPath(), command, arguments));

    ready(thread);
    return process;
}

void SchedulerService::lockScheduler() {
    scheduler.lock.acquire();
}

void SchedulerService::unlockScheduler() {
    scheduler.lock.release();
}

void SchedulerService::yield() {
    scheduler.yield();
}

Kernel::Process& SchedulerService::getCurrentProcess() {
    return scheduler.getCurrentThread().getParent();
}

Thread& SchedulerService::getCurrentThread() {
    return scheduler.getCurrentThread();
}

void SchedulerService::cleanup(Process *process) {
    cleaner->cleanup(process);
}

void SchedulerService::cleanup(Thread *thread) {
    fpu->checkTerminatedThread(*thread);
    cleaner->cleanup(thread);
}

bool SchedulerService::isProcessActive(uint32_t id) {
    for (const auto *process : processList) {
        if (process->getId() == id) {
            return true;
        }
    }

    return false;
}

void SchedulerService::exitCurrentProcess(int32_t exitCode) {
    auto &process = getCurrentProcess();
    auto &cleanerThread = Thread::createKernelThread("Address-Space-Cleaner", process, new AddressSpaceCleaner());

    process.killAllThreadsButCurrent();
    ready(cleanerThread);

    process.setExitCode(exitCode);

    processLock.acquire();
    processList.remove(&process);
    processLock.release();

    scheduler.exit();

    __builtin_unreachable();
}

void SchedulerService::block() {
    scheduler.block();
}

void SchedulerService::unblock(Thread &thread) {
    auto &process = thread.getParent();
    if (isProcessActive(process.getId())) {
        scheduler.unblock(thread);
    } else {
        cleanup(&thread);
    }
}

void SchedulerService::kill(Thread &thread) {
    scheduler.kill(thread);
}

uint8_t *SchedulerService::getDefaultFpuContext() {
    return defaultFpuContext;
}

Process* SchedulerService::getProcess(uint32_t id) {
    for (auto *process : processList) {
        if (process->getId() == id) {
            return process;
        }
    }

    return nullptr;
}

Process& SchedulerService::getKernelProcess() const {
    return kernelProcess;
}

void SchedulerService::sleep(const Util::Time::Timestamp &time) {
    scheduler.sleep(time);
}

}