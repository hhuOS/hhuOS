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
#include "lib/util/stream/FileOutputStream.h"
#include "lib/util/stream/BufferedOutputStream.h"
#include "lib/util/stream/PrintWriter.h"

namespace Util {

class System {

public:

    enum Code : uint16_t {
        SCHEDULER_YIELD,
        EXIT_PROCESS,
        EXECUTE_BINARY,
        IS_PROCESS_ACTIVE,
        UNMAP,
        MAP_IO,
        CREATE_FILE,
        DELETE_FILE,
        OPEN_FILE,
        CLOSE_FILE,
        FILE_TYPE,
        FILE_LENGTH,
        FILE_CHILDREN,
        WRITE_FILE,
        READ_FILE,
        CHANGE_DIRECTORY,
        GET_CURRENT_WORKING_DIRECTORY,
        GET_SYSTEM_TIME,
        SET_DATE,
        GET_CURRENT_DATE
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
     * Copy constructor.
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

    static Util::Stream::PrintWriter out;
    static const char *errorMessage;

private:

    static void call(Code code, Result &result, uint32_t paramCount, va_list args);

    static Util::Stream::FileOutputStream outStream;
    static Util::Stream::BufferedOutputStream bufferedOutStream;

};

}

#endif
