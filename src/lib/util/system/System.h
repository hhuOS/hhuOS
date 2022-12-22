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

#ifndef HHUOS_USER_SYSTEMCALL_H
#define HHUOS_USER_SYSTEMCALL_H

#include <cstdint>
#include <cstdarg>

#include "lib/util/stream/InputStreamReader.h" // IWYU pragma: keep

namespace Util {
namespace Stream {
class BufferedInputStream;
class BufferedOutputStream;
class FileInputStream;
class FileOutputStream;
class PrintWriter;
}  // namespace Stream

class System {

public:

    enum Code : uint16_t {
        YIELD,
        EXIT_PROCESS,
        EXECUTE_BINARY,
        GET_CURRENT_PROCESS,
        GET_CURRENT_THREAD,
        JOIN_THREAD,
        CREATE_THREAD,
        EXIT_THREAD,
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

    enum Result : uint16_t {
        OK,
        INVALID_ARGUMENT,
        OUT_OF_BOUNDS,
        ILLEGAL_STATE,
        NOT_INITIALIZED
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

    static Result call(Code code, uint32_t paramCount...);

    static Util::Stream::InputStreamReader in;
    static Util::Stream::PrintWriter out;
    static Util::Stream::PrintWriter error;

    static const char *errorMessage;

private:

    static void call(Code code, Result &result, uint32_t paramCount, va_list args);

    static Util::Stream::FileInputStream inStream;
    static Util::Stream::BufferedInputStream bufferedInStream;

    static Util::Stream::FileOutputStream outStream;
    static Util::Stream::BufferedOutputStream bufferedOutStream;

    static Util::Stream::FileOutputStream errorStream;
    static Util::Stream::BufferedOutputStream bufferedErrorStream;
};

}

#endif
