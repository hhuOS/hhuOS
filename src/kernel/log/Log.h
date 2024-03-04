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
 
#ifndef LOG_H
#define LOG_H

#include <cstdarg>
#include "device/port/serial/SerialPort.h"

namespace Kernel {

class Log {

    enum Level {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4
    };

    struct Record {
        Level level;
        const char *file;
        const char *line;
        va_list arguments;
    };

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Log() = delete;

    /**
     * Copy Constructor.
     */
    Log(const Log &other) = delete;

    /**
     * Assignment operator.
     */
    Log &operator=(const Log &other) = delete;

    /**
     * Destructor.
     * Deleted, as this class has only static members.
     */
    ~Log() = delete;

    static void log(const char *message, const Record &record);

private:

    static Device::SerialPort serial;
};

}

#endif
