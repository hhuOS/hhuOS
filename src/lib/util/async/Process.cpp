/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Process.h"

#include "lib/interface.h"
#include "base/System.h"

namespace Util {

template <typename T> class Array;
class String;

namespace Io {
class File;
}  // namespace File
}  // namespace Util

namespace Util::Async {

Process::Process(const size_t id) : id(id) {}

Process Process::execute(const Io::File &binaryFile, const Io::File &inputFile, const Io::File &outputFile,
    const Io::File &errorFile, const String &command, const Array<String> &arguments)
{
    return executeBinary(binaryFile, inputFile, outputFile, errorFile, command, arguments);
}

Process Process::getCurrentProcess() {
    return ::getCurrentProcess();
}

void Process::exit(int32_t exitCode) {
    System::call(System::System::EXIT_PROCESS, 1, exitCode);
}

void Process::join() const {
    joinProcess(id);
}

void Process::kill() const {
    killProcess(id);
}

size_t Process::getId() const {
    return id;
}

}