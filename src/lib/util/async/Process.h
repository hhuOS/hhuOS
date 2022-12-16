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

#ifndef HHUOS_UTIL_PROCESS_H
#define HHUOS_UTIL_PROCESS_H

#include <cstdint>

#include "lib/util/memory/String.h"

namespace Util {
namespace Data {
template <typename T> class Array;
}  // namespace Data
namespace File {
class File;
}  // namespace File
}  // namespace Util

namespace Util::Async {

class Process {

public:
    /**
     * Constructor.
     */
    explicit Process(uint32_t id);

    /**
     * Copy Constructor.
     */
    Process(const Process &other) = default;

    /**
     * Assignment operator.
     */
    Process &operator=(const Process &other) = default;

    /**
     * Destructor.
     */
    ~Process() = default;

    static Process execute(const File::File &binaryFile, const File::File &inputFile, const File::File &outputFile, const File::File &errorFile, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments);

    static Process getCurrentProcess();

    static void yield();

    static void exit(int32_t exitCode);

    [[nodiscard]] uint32_t getId() const;

    void join() const;

    void kill() const;

private:

    uint32_t id;
};

}

#endif