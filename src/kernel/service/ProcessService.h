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

#ifndef HHUOS_PROCESSSERVICE_H
#define HHUOS_PROCESSSERVICE_H

#include <cstdint>

#include "Service.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/base/String.h"
#include "kernel/process/Process.h"

namespace Util {
namespace Io {
class File;
}  // namespace File
}  // namespace Util

namespace Kernel {
class VirtualAddressSpace;

class ProcessService : public Service {

public:
    /**
     * Default Constructor.
     */
    ProcessService();

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

    [[nodiscard]] bool isProcessActive(uint32_t id);

    [[nodiscard]] Process& getCurrentProcess();

    [[nodiscard]] Process* getProcess(uint32_t id);

    [[nodiscard]] Process& getKernelProcess() const;

    [[nodiscard]] Util::Array<uint32_t> getActiveProcessIds() const;

    static const constexpr uint8_t SERVICE_ID = 7;

private:

    Util::ArrayList<Process*> processList;
    Util::Async::Spinlock lock;
    Process &kernelProcess;
};

}

#endif
