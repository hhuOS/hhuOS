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

#ifndef HHUOS_PROCESS_H
#define HHUOS_PROCESS_H

#include <cstdint>

#include "lib/util/io/file/File.h"
#include "kernel/file/FileDescriptorManager.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/base/String.h"
#include "kernel/process/Thread.h"

namespace Util {
namespace Async {
template <typename T> class IdGenerator;
}  // namespace Async
}  // namespace Util

namespace Kernel {
class VirtualAddressSpace;

class Process {

public:
    /**
     * Constructor.
     */
    explicit Process(VirtualAddressSpace &addressSpace, const Util::String &name, const Util::Io::File &workingDirectory = Util::Io::File("/"));

    /**
     * Copy Constructor.
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

    bool operator==(const Process &other) const;

    bool setWorkingDirectory(const Util::String &path);

    void setExitCode(int32_t code);

    void setMainThread(Thread &thread);

    void join();

    [[nodiscard]] uint32_t getId() const;

    [[nodiscard]] uint32_t getThreadCount() const;

    [[nodiscard]] VirtualAddressSpace& getAddressSpace();

    [[nodiscard]] FileDescriptorManager& getFileDescriptorManager();

    [[nodiscard]] Util::Io::File getWorkingDirectory();

    [[nodiscard]] bool isFinished() const;

    [[nodiscard]] int32_t getExitCode() const;

    [[nodiscard]] bool isKernelProcess() const;

    [[nodiscard]] Util::String getName() const;

    [[nodiscard]] Util::Array<Thread*> getThreads() const;

    void addThread(Thread &thread);

    void removeThread(Thread &thread);

    void killAllThreadsButCurrent();

private:

    [[nodiscard]] Util::Io::File getFileFromPath(const Util::String &path);

    uint32_t id;
    Util::String name;
    VirtualAddressSpace &addressSpace;
    FileDescriptorManager fileDescriptorManager;
    Util::Io::File workingDirectory;
    Util::ArrayList<Thread*> threads;
    Thread *mainThread = nullptr;

    bool finished = false;
    int32_t exitCode = -1;

    static Util::Async::IdGenerator<uint32_t> idGenerator;
};

}

#endif
