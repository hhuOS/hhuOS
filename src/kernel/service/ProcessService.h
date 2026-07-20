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

#ifndef HHUOS_PROCESSSERVICE_H
#define HHUOS_PROCESSSERVICE_H

#include <stdint.h>

#include "Service.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/base/String.h"
#include "kernel/process/Scheduler.h"

namespace Util {
namespace Io {
class File;
}  // namespace File
}  // namespace Util

namespace Kernel {
class VirtualAddressSpace;
class SchedulerCleaner;
class Thread;
class Process;

class ProcessService : public Service {

public:
    /**
     * Constructor.
     */
    explicit ProcessService(Process *kernelProcess);

    /**
     * Copy Constructor.
     */
    ProcessService(const ProcessService &other) = delete;

    /**
     * Assignment operator.
     */
    ProcessService &operator=(const ProcessService &other) = delete;

    /**
     * Destructor.
     */
    ~ProcessService() override = default;

    Process& createProcess(VirtualAddressSpace &addressSpace, const Util::String &name, const Util::Io::File &workingDirectory, const Util::Io::File &standardIn, const Util::Io::File &standardOut, const Util::Io::File &standardError);

    Process& loadBinary(const Util::Io::File &binaryFile, const Util::Io::File &inputFile, const Util::Io::File &outputFile, const Util::Io::File &errorFile, const Util::String &command, const Util::Array<Util::String> &arguments);

    void killProcess(Process &process);

    [[noreturn]] void exitCurrentProcess(int32_t exitCode);

    bool isProcessActive(uint32_t id);

    Process& getCurrentProcess();

    Process* getProcess(uint32_t id);

    Process& getKernelProcess() const;

    Util::Array<uint32_t> getActiveProcessIds() const;

    Scheduler& getScheduler();

    void cleanup(Thread *thread);

    void cleanup(Process *process);

    void startScheduler();

    static const constexpr uint8_t SERVICE_ID = 7;

private:

    static int64_t systemCallYield();

    static int64_t systemCallGetCurrentThread();

    static int64_t systemCallCreateThread(const char *name, Util::Async::Runnable *runnable, uint32_t eip);

    static int64_t systemCallSleep(const Util::Time::Timestamp *time);

    static int64_t systemCallJoinThread(uint32_t threadId);

    static int64_t systemCallExitThread();

    static int64_t systemCallExitProcess(int32_t exitCode);

    static int64_t systemCallExecuteBinary(const char *binaryPath, const char **stdPaths, const char *command, uint32_t argc, const char **argv);

    static int64_t systemCallGetCurrentProcess();

    static int64_t systemCallJoinProcess(uint32_t processId);

    static int64_t systemCallKillProcess(uint32_t processId);

    static int64_t systemCallCreatePipe(const char *name);

    static int64_t systemCallSharedMemory(const char *name, void *startAddress, uint32_t pageCount);

    Scheduler scheduler;
    SchedulerCleaner *cleaner = nullptr;

    Util::ArrayList<Process*> processList;
    Util::Async::Spinlock lock;
    Process *kernelProcess;
};

}

#endif
