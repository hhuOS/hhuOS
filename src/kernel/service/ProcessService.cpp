/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include <stdarg.h>

#include "kernel/process/BinaryLoader.h"
#include "ProcessService.h"
#include "FilesystemService.h"
#include "kernel/process/FileDescriptorManager.h"
#include "kernel/process/Process.h"
#include "kernel/process/Thread.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/base/Panic.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/System.h"
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

    ASSIGN_SYSTEM_CALL(Util::System::YIELD, ProcessService::systemCallYield);
    ASSIGN_SYSTEM_CALL(Util::System::GET_CURRENT_THREAD, ProcessService::systemCallGetCurrentThread);
    ASSIGN_SYSTEM_CALL(Util::System::CREATE_THREAD, ProcessService::systemCallCreateThread);
    ASSIGN_SYSTEM_CALL(Util::System::SLEEP, ProcessService::systemCallSleep);
    ASSIGN_SYSTEM_CALL(Util::System::JOIN_THREAD, ProcessService::systemCallJoinThread);
    ASSIGN_SYSTEM_CALL(Util::System::EXIT_THREAD, ProcessService::systemCallExitThread);
    ASSIGN_SYSTEM_CALL(Util::System::EXIT_PROCESS, ProcessService::systemCallExitProcess);
    ASSIGN_SYSTEM_CALL(Util::System::EXECUTE_BINARY, ProcessService::systemCallExecuteBinary);
    ASSIGN_SYSTEM_CALL(Util::System::GET_CURRENT_PROCESS, ProcessService::systemCallGetCurrentProcess);
    ASSIGN_SYSTEM_CALL(Util::System::JOIN_PROCESS, ProcessService::systemCallJoinProcess);
    ASSIGN_SYSTEM_CALL(Util::System::KILL_PROCESS, ProcessService::systemCallKillProcess);
    ASSIGN_SYSTEM_CALL(Util::System::CREATE_PIPE, ProcessService::systemCallCreatePipe);
    ASSIGN_SYSTEM_CALL(Util::System::DESTROY_PIPE, ProcessService::systemCallDestroyPipe);
    ASSIGN_SYSTEM_CALL(Util::System::CREATE_SHARED_MEMORY, ProcessService::systemCallSharedMemory);
    ASSIGN_SYSTEM_CALL(Util::System::DESTROY_SHARED_MEMORY, ProcessService::systemCallDestroySharedMemory);
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
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "A process cannot kill itself!");
    }

    for (auto *thread : process.getThreads()) {
        scheduler.kill(*thread);
    }

    process.setExitCode(-1);

    lock.acquire();
    processList.remove(&process);
    lock.release();

    cleanup(&process);
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
    process.killAllThreadsButCurrent();
    process.setExitCode(exitCode);

    lock.acquire();
    processList.remove(&process);
    lock.release();

    cleanup(&process);

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

int64_t ProcessService::systemCallYield() {
    auto &processService = getService<ProcessService>();
    
    processService.getScheduler().yield();
    return 0;
}

int64_t ProcessService::systemCallGetCurrentThread() {
    auto &processService = getService<ProcessService>();

    return processService.getScheduler().getCurrentThread().getId();
}

int64_t ProcessService::systemCallCreateThread(const char *name, Util::Async::Runnable *runnable, const uint32_t eip) {
    auto &processService = getService<ProcessService>();

    auto &thread = Thread::createUserThread(name, processService.getCurrentProcess(), eip, runnable);
    processService.getScheduler().ready(thread);
    
    return thread.getId();
}

int64_t ProcessService::systemCallSleep(const Util::Time::Timestamp* time) {
    auto &processService = getService<ProcessService>();

    processService.getScheduler().sleep(*time);
    return 0;
}

int64_t ProcessService::systemCallJoinThread(const uint32_t threadId) {
    auto &processService = getService<ProcessService>();

    auto *thread = processService.getScheduler().getThread(threadId);
    if (thread == nullptr) {
        return -1;
    }
    
    thread->join();
    return 0;
}

int64_t ProcessService::systemCallExitThread() {
    auto &processService = getService<ProcessService>();
    
    processService.getScheduler().exit();
    return 0;
}

int64_t ProcessService::systemCallExitProcess(const int32_t exitCode) {
    auto &processService = getService<ProcessService>();
    
    processService.exitCurrentProcess(exitCode);
    return 0;
}

int64_t ProcessService::systemCallExecuteBinary(const char *binaryPath, const char **stdPaths, const char *command, const uint32_t argc, const char **argv) {
    auto &processService = getService<ProcessService>();
    const Util::Io::File binaryFile(binaryPath);
    const Util::Io::File inputFile(stdPaths[0]);
    const Util::Io::File outputFile(stdPaths[1]);
    const Util::Io::File errorFile(stdPaths[2]);
    Util::Array<Util::String> commandArguments(argc);

    for (uint32_t i = 0; i < argc; i++) {
        commandArguments[i] = argv[i];
    }

    auto &process = processService.loadBinary(binaryFile, inputFile, outputFile, errorFile, command, commandArguments);
    return process.getId();
}

int64_t ProcessService::systemCallGetCurrentProcess() {
    auto &processService = getService<ProcessService>();
    
    return processService.getCurrentProcess().getId();
}

int64_t ProcessService::systemCallJoinProcess(const uint32_t processId) {
    auto &processService = getService<ProcessService>();

    auto *process = processService.getProcess(processId);
    if (process == nullptr) {
        return -1;
    }

    process->join();
    return 0;
}

int64_t ProcessService::systemCallKillProcess(const uint32_t processId) {
    auto &processService = getService<ProcessService>();

    auto *process = processService.getProcess(processId);
    if (process == nullptr) {
        return -1;
    }

    processService.killProcess(*process);
    return 0;
}

int64_t ProcessService::systemCallCreatePipe(const char *name) {
    auto &processService = getService<ProcessService>();
    auto &currentProcess = processService.getCurrentProcess();

    return currentProcess.createPipe(name) ? 0 : -1;
}

int64_t ProcessService::systemCallDestroyPipe(const char *name) {
    auto &processService = getService<ProcessService>();
    auto &currentProcess = processService.getCurrentProcess();

    return currentProcess.destroyPipe(name);
}

int64_t ProcessService::systemCallSharedMemory(const char *name, void *startAddress, const uint32_t pageCount) {
    auto &processService = getService<ProcessService>();
    auto &currentProcess = processService.getCurrentProcess();

    return currentProcess.createSharedMemory(name, startAddress, pageCount) ? 0 : -1;
}

int64_t ProcessService::systemCallDestroySharedMemory(const char *name) {
    auto &processService = getService<ProcessService>();
    auto &currentProcess = processService.getCurrentProcess();

    return currentProcess.destroySharedMemory(name);
}

}
