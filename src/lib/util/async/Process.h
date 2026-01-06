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

#ifndef HHUOS_LIB_UTIL_ASYNC_PROCESS_H
#define HHUOS_LIB_UTIL_ASYNC_PROCESS_H

#include <stddef.h>

#include "util/base/String.h"
#include "util/collection/Array.h"
#include "util/io/file/File.h"

namespace Util {
namespace Async {

/// Create and manipulate processes from the user space.
/// This class just wraps a process ID and uses systems calls to manipulate the process referenced by the ID.
class Process {

public:
    /// Create an instance with the given ID.
    /// This constructor does not create a new process, it just wraps the given ID.
    explicit Process(const size_t id) : id(id) {}

    /// Load a program from an executable file and run it as a new process.
    ///
    /// @param binaryFile The executable file to load.
    /// @param inputFile The standard input file for the process (e.g. "/device/terminal").
    /// @param outputFile The standard output file for the process (e.g "/device/terminal").
    /// @param errorFile The standard error file for the process (e.g "/device/terminal").
    /// @param command The path/name that the program was called with.
    /// @param arguments The arguments to pass to the program.
    ///
    /// ### Example
    /// ```c++
    /// auto echoProcess = Util::Async::Process::execute(
    ///     "/bin/echo", "/dev/terminal", "/dev/terminal", "/dev/terminal", "echo", {"Hello", "World"});
    /// ```
    static Process execute(const Io::File &binaryFile, const Io::File &inputFile, const Io::File &outputFile,
        const Io::File &errorFile, const String &command, const Array<String> &arguments);

    /// Get access to the current process.
    ///
    /// ### Example
    /// ```c++
    /// auto process = Util::Async::Process::getCurrentProcess();
    /// process.exit(0); // Exit the current process with exit code 0
    /// ```
    static Process getCurrentProcess();

    /// Exit the current process.
    /// This function does not return.
    static void exit(int32_t exitCode);

    /// Join the process by blocking until it has finished.
    ///
    /// ### Example
    /// ```c++
    /// auto echoProcess = Util::Async::Process::execute(
    ///     "/bin/echo", "/dev/terminal", "/dev/terminal", "/dev/terminal", "echo", {"Hello", "World"});
    ///
    /// echoProcess.join(); // Wait for the process to finish
    /// Util::System::out << "Process 'echo' has finished!"
    ///     << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    /// ```
    void join() const;

    /// Kill the process.
    /// The current process cannot be killed. Use `exit()` instead.
    void kill() const;

    /// Get the ID of the process.
    size_t getId() const;

private:

    const size_t id;
};

}
}

#endif