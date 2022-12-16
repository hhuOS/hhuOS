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

#include "lib/util/system/System.h"
#include "lib/interface.h"
#include "lib/util/async/Process.h"

namespace Util {
namespace Data {
template <typename T> class Array;
}  // namespace Data
namespace File {
class File;
}  // namespace File
}  // namespace Util

namespace Util::Async {

Process::Process(uint32_t id) : id(id) {}

Process Process::execute(const File::File &binaryFile, const File::File &inpuputFile, const File::File &outputFile, const File::File &errorFile, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments) {
    return ::executeBinary(binaryFile, inpuputFile, outputFile, errorFile, command, arguments);
}

Process Process::getCurrentProcess() {
    return ::getCurrentProcess();
}

void Process::yield() {
    System::System::call(System::System::YIELD, 0);
}

void Process::exit(int32_t exitCode) {
    System::System::call(System::System::EXIT_PROCESS, 1, exitCode);
}

uint32_t Process::getId() const {
    return id;
}

void Process::join() const {
    ::joinProcess(id);
}

void Process::kill() const {
    ::killProcess(id);
}

}