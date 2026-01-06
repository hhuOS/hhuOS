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

#include "Process.h"

#include "interface.h"
#include "util/base/System.h"

namespace Util {

template <typename T> class Array;
class String;

namespace Io {
class File;
}  // namespace File
}  // namespace Util

namespace Util {
namespace Async {

Process Process::execute(const Io::File &binaryFile, const Io::File &inputFile, const Io::File &outputFile,
    const Io::File &errorFile, const String &command, const Array<String> &arguments)
{
    return executeBinary(binaryFile, inputFile, outputFile, errorFile, command, arguments);
}

Process Process::getCurrentProcess() {
    return ::getCurrentProcess();
}

void Process::exit(const int32_t exitCode) {
    System::call(System::EXIT_PROCESS, 1, exitCode);
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
}