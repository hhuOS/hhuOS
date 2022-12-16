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

#ifndef HHUOS_PROCESSSERVICE_H
#define HHUOS_PROCESSSERVICE_H

#include <cstdint>

#include "Service.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"
#include "lib/util/memory/String.h"

namespace Util {
namespace File {
class File;
}  // namespace File
}  // namespace Util

namespace Kernel {
class Process;
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

    Process& createProcess(VirtualAddressSpace &addressSpace, const Util::Memory::String &name, const Util::File::File &workingDirectory, const Util::File::File &standardIn, const Util::File::File &standardOut, const Util::File::File &standardError);

    Process& loadBinary(const Util::File::File &binaryFile, const Util::File::File &inputFile, const Util::File::File &outputFile, const Util::File::File &errorFile, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments);

    void killProcess(Process &process);

    [[noreturn]] void exitCurrentProcess(int32_t exitCode);

    [[nodiscard]] bool isProcessActive(uint32_t id);

    [[nodiscard]] Process& getCurrentProcess();

    [[nodiscard]] Process* getProcess(uint32_t id);

    [[nodiscard]] Process& getKernelProcess() const;

    [[nodiscard]] Util::Data::Array<uint32_t> getActiveProcessIds() const;

    static const constexpr uint8_t SERVICE_ID = 7;

private:

    Util::Data::ArrayList<Process*> processList;
    Util::Async::Spinlock lock;
    Process &kernelProcess;
};

}

#endif
