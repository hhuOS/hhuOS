/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_USER_SYSTEMCALL_H
#define HHUOS_USER_SYSTEMCALL_H

#include <cstdint>
#include <cstdarg>

#include "lib/util/io/stream/InputStream.h" // IWYU pragma: keep
#include "lib/util/io/stream/PrintStream.h" // IWYU pragma: keep

namespace Util {
namespace Io {
class BufferedInputStream;
class BufferedOutputStream;
class FileInputStream;
class FileOutputStream;
}  // namespace Stream

class System {

public:

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

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    System() = delete;

    /**
     * Copy Constructor.
     */
    System(const System &other) = delete;

    /**
     * Assignment operator.
     */
    System &operator=(const System &other) = delete;

    /**
     * Destructor.
     */
    ~System() = default;

    static bool call(Code code, uint32_t paramCount...);

    static void printStackTrace(const Util::Io::PrintStream &stream, uint32_t minEbp, bool userSpace);

    static Io::InputStream &in;
    static Io::PrintStream out;
    static Io::PrintStream error;

private:

    static void call(Code code, bool &result, uint32_t paramCount, va_list args);

    static const char* getSymbolName(uint32_t symbolAddress);

    static Io::FileInputStream inStream;
    static Io::BufferedInputStream bufferedInStream;

    static Io::FileOutputStream outStream;
    static Io::BufferedOutputStream bufferedOutStream;

    static Io::FileOutputStream errorStream;
    static Io::BufferedOutputStream bufferedErrorStream;
};

}

#endif
