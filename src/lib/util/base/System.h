/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_UTIL_SYSTEMCALL_H
#define HHUOS_LIB_UTIL_SYSTEMCALL_H

#include <stddef.h>
#include <stdarg.h>

#include "lib/util/io/stream/InputStream.h" // IWYU pragma: keep
#include "lib/util/io/stream/PrintStream.h" // IWYU pragma: keep
#include "FreeListMemoryManager.h"

namespace Util {
namespace Io {
class BufferedInputStream;
class BufferedOutputStream;
class FileInputStream;
class FileOutputStream;
namespace Elf {
struct SymbolEntry;
}  // namespace Elf
}  // namespace Stream

/// Provides system calls for user space applications and gives access to the standard input and output streams.
class System {

public:
    /// This class has only static members and is not meant to be instantiated, so the constructor is deleted.
    System() = delete;

    /// This class has only static members and is not meant to be instantiated, so the copy constructor is deleted.
    System(const System &other) = delete;

    /// This class has only static members and is not meant to be instantiated, so the assignment operator is deleted.
    System &operator=(const System &other) = delete;

    /// This class has only static members and is not meant to be instantiated, so the destructor is deleted.
    ~System() = delete;

    /// System call codes
    enum Code : uint8_t {
        YIELD,
        EXIT_PROCESS,
        EXECUTE_BINARY,
        GET_CURRENT_PROCESS,
        GET_CURRENT_THREAD,
        JOIN_THREAD,
        CREATE_THREAD,
        EXIT_THREAD,
        KILL_THREAD,
        JOIN_PROCESS,
        KILL_PROCESS,
        SLEEP,
        UNMAP,
        MAP_IO,
        MOUNT,
        UNMOUNT,
        CREATE_FILE,
        DELETE_FILE,
        OPEN_FILE,
        CLOSE_FILE,
        CONTROL_FILE_DESCRIPTOR,
        FILE_TYPE,
        FILE_LENGTH,
        FILE_CHILDREN,
        WRITE_FILE,
        READ_FILE,
        CONTROL_FILE,
        CREATE_SOCKET,
        SEND_DATAGRAM,
        RECEIVE_DATAGRAM,
        CHANGE_DIRECTORY,
        GET_CURRENT_WORKING_DIRECTORY,
        GET_SYSTEM_TIME,
        SET_DATE,
        GET_CURRENT_DATE,
        SHUTDOWN
    };

    /// Every address space has this struct placed at `Util::USER_SPACE_MEMORY_START_ADDRESS`.
    /// It contains the heap memory manager for this user space and a pointer the symbol table of the loaded program.
    struct AddressSpaceHeader {
        /// The heap memory manager for this user space.
        FreeListMemoryManager memoryManager;

        /// The symbol table size (in bytes) of the loaded program.
        size_t symbolTableSize;

        /// A pointer to the symbol table of the loaded program.
        const Io::Elf::SymbolEntry *symbolTable;

        /// A pointer to the string table of the loaded program.
        const char *stringTable;
    };

    /// Perform a system call.
    /// The variables for each call may differ in size and type
    /// and must be passed as a variadic list of size_t parameters.
    /// There are high-level APIs for each system call, which should be used instead of this function.
    ///
    /// ### Example
    /// ```c++
    /// // Create the file "/user/test.txt". Usually, this would be done using the Util::Io::File class.
    /// Util::System::call(Util::System::CREATE_FILE, 2, "/user/test.txt", Util::Io::File::Type::REGULAR);
    /// ```
    static bool call(Code code, size_t paramCount...);

    /// Print the current stack trace to a given stream.
    /// This only works for user space applications, not for the kernel.
    /// See 'lib/kernel.cpp' and 'kernel/service/InformationService' for kernel stack traces.
    static void printStackTrace(Io::PrintStream &stream, size_t minEbp);

    /// Get the address space header of the current process.
    [[nodiscard]] static AddressSpaceHeader& getAddressSpaceHeader();

    /// The standard input stream, used for reading user input.
    ///
    /// ### Example
    /// ```c++
    /// // Read a line from the standard input and print it to the standard output.
    /// auto input = Util::System::in.readLine();
    /// Util::System::out << "Input: " << input << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// // Read characters individually from the standard input.
    /// auto c = Util::System::in.read();
    /// while (c != Util::Io::FileStream::END_OF_FILE) {
    ///     Util::System::out << c << Util::Io::PrintStream::flush;
    ///     c = Util::System::in.read();
    /// }
    /// ```
    static Io::InputStream &in;

    /// The standard output stream, used for printing to the terminal.
    ///
    /// ### Example
    /// ```c++
    /// Util::System::out << "Hello, World!" << Io::PrintStream::endl << Io::PrintStream::flush;
    /// ```
    static Io::PrintStream out;

    /// The standard error stream, used for printing error messages to the terminal.
    ///
    /// ### Example
    /// ```c++
    /// Util::System::error << "File not found!" << Io::PrintStream::endl << Io::PrintStream::flush;
    /// ```
    static Io::PrintStream error;
};

}

#endif
