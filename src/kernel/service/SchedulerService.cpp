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

void SchedulerService::kickoffThread() {
    scheduler.getCurrentProcess().getThreadScheduler().getCurrentThread().run();
    scheduler.getCurrentProcess().getThreadScheduler().exit();
}

void SchedulerService::startScheduler() {
    cleaner = new Kernel::SchedulerCleaner();
    auto &schedulerCleanerThread = Kernel::Thread::createKernelThread("Scheduler Cleaner", cleaner);
    ready(schedulerCleanerThread);

    defaultFpuContext = static_cast<uint8_t*>(System::getService<MemoryService>().allocateKernelMemory(512, 16));
    Util::Memory::Address<uint32_t>(defaultFpuContext).setRange(0, 512);

    if (Device::Fpu::isAvailable()) {
        log.info("FPU detected -> Enabling FPU context switching");
        fpu = new Device::Fpu();
        fpu->plugin();
    } else {
        log.warn("No FPU present");
    }
    
    scheduler.start();
}

void SchedulerService::ready(Process &process) {
    scheduler.ready(process);
}

void SchedulerService::ready(Thread &thread) {
    scheduler.getCurrentProcess().ready(thread);
}

Process& SchedulerService::createProcess(VirtualAddressSpace &addressSpace, const Util::File::File &workingDirectory, const Util::File::File &standardOut) {
    auto *process = new Process(scheduler, addressSpace, workingDirectory);
    // Create standard out file descriptor
    if (System::isServiceRegistered(FilesystemService::SERVICE_ID)) {
        process->getFileDescriptorManager().openFile(standardOut.getCanonicalPath());
    }

    return *process;
}

Process &SchedulerService::loadBinary(const Util::File::File &binaryFile, const Util::File::File &outputFile, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments) {
    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();

    auto &virtualAddressSpace = memoryService.createAddressSpace();
    auto &process = createProcess(virtualAddressSpace, Util::File::getCurrentWorkingDirectory(), outputFile);
    auto &thread = Kernel::Thread::createKernelThread("Loader", new Kernel::BinaryLoader(binaryFile.getCanonicalPath(), command, arguments));

    process.ready(thread);
    ready(process);

    return process;
}

void SchedulerService::lockScheduler() {
    scheduler.lock.acquire();
}

void SchedulerService::unlockScheduler() {
    scheduler.lock.release();
}

void SchedulerService::setSchedulerInitialized() {
    if (scheduler.isInitialized()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Scheduler is already initialized!");
    }

    SystemCall::registerSystemCall(Util::System::SCHEDULER_YIELD, [](uint32_t, va_list) -> Util::System::Result {
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

        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::EXECUTE_BINARY, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 5) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto *binaryFile = va_arg(arguments, Util::File::File*);
        auto *outputFile = va_arg(arguments, Util::File::File*);
        auto *command = va_arg(arguments, const Util::Memory::String*);
        auto *commandArguments = va_arg(arguments, Util::Data::Array<Util::Memory::String>*);
        auto *processId = va_arg(arguments, uint32_t*);

        auto &schedulerService = System::getService<SchedulerService>();
        auto &process = schedulerService.loadBinary(*binaryFile, *outputFile, *command, *commandArguments);

        *processId = process.getId();
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::IS_PROCESS_ACTIVE, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 2) {
            return Util::System::INVALID_ARGUMENT;
        }

        auto processId = va_arg(arguments, uint32_t);
        auto *isActive = va_arg(arguments, bool*);

        auto &schedulerService = System::getService<SchedulerService>();
        *isActive = schedulerService.isProcessActive(processId);

        return Util::System::Result::OK;
    });

    scheduler.setInitialized();
}

void SchedulerService::yield() {
    if (scheduler.isInitialized()) {
        scheduler.yield();
    }
}

bool SchedulerService::isSchedulerInitialized() const {
    return scheduler.isInitialized();
}

bool SchedulerService::isProcessActive(uint32_t id) {
    return scheduler.isProcessActive(id);
}

Kernel::Process& SchedulerService::getCurrentProcess() {
    return scheduler.getCurrentProcess();
}

Thread& SchedulerService::getCurrentThread() {
    return scheduler.getCurrentProcess().getThreadScheduler().getCurrentThread();
}

void SchedulerService::cleanup(Process *process) {
    cleaner->cleanup(process);
}

void SchedulerService::cleanup(Thread *thread) {
    fpu->checkTerminatedThread(*thread);
    cleaner->cleanup(thread);
}

void SchedulerService::exitCurrentProcess(int32_t exitCode) {
    getCurrentProcess().getThreadScheduler().killAllThreadsButCurrent();
    auto &cleanerThread = Thread::createKernelThread("Cleaner", new AddressSpaceCleaner());
    ready(cleanerThread);
    getCurrentProcess().setExitCode(exitCode);
    getCurrentProcess().getThreadScheduler().exit();
}

void SchedulerService::block() {
    scheduler.blockCurrentThread();
}

void SchedulerService::unblock(Thread &thread) {
    scheduler.unblockThread(thread);
}

void SchedulerService::kill(Thread &thread) {
    scheduler.getCurrentProcess().getThreadScheduler().kill(thread);
}

uint8_t *SchedulerService::getDefaultFpuContext() {
    return defaultFpuContext;
}

}