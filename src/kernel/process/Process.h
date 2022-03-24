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

#ifndef HHUOS_PROCESS_H
#define HHUOS_PROCESS_H

#include "kernel/paging/VirtualAddressSpace.h"
#include "lib/util/file/File.h"
#include "Thread.h"
#include "ThreadScheduler.h"

namespace Kernel {

class Process {

public:
    /**
     * Constructor.
     */
    explicit Process(ProcessScheduler &scheduler, VirtualAddressSpace &addressSpace, const Util::File::File &workingDirectory = Util::File::File("/"));

    /**
     * Copy constructor.
     */
    Process(const Process &other) = delete;

    /**
     * Assignment operator.
     */
    Process &operator=(const Process &other) = delete;

    /**
     * Destructor.
     */
    ~Process();

    void ready(Thread &thread);

    void start();

    void exit();

    bool setWorkingDirectory(const Util::Memory::String &path);

    void setExitCode(int32_t code);

    [[nodiscard]] uint32_t getId() const;

    [[nodiscard]] uint32_t getThreadCount() const;

    [[nodiscard]] VirtualAddressSpace& getAddressSpace();

    [[nodiscard]] ThreadScheduler& getThreadScheduler();

    [[nodiscard]] FileDescriptorManager& getFileDescriptorManager();

    [[nodiscard]] Util::File::File getWorkingDirectory();

    [[nodiscard]] bool isFinished() const;

    [[nodiscard]] int32_t getExitCode() const;

    [[nodiscard]] bool isKernelProcess() const;

private:

    [[nodiscard]] Util::File::File getFileFromPath(const Util::Memory::String &path);

    uint32_t id;
    VirtualAddressSpace &addressSpace;
    ProcessScheduler &scheduler;
    ThreadScheduler threadScheduler;
    FileDescriptorManager fileDescriptorManager;
    Util::File::File workingDirectory;

    bool finished = false;
    int32_t exitCode = -1;

    static Util::Async::IdGenerator<uint32_t> idGenerator;
};

}

#endif
