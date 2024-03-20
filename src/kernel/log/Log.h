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

#include "lib/util/base/Constants.h"

#define LOG_XSTRINGIFY(a) LOG_STRINGIFY(a)
#define LOG_STRINGIFY(a) #a

#define LOG_TRACE(message...) Kernel::Log::log(Kernel::Log::Record{ Kernel::Log::TRACE, __FILE__, LOG_XSTRINGIFY(__LINE__) }, message)
#define LOG_DEBUG(message...) Kernel::Log::log(Kernel::Log::Record{ Kernel::Log::DEBUG, __FILE__, LOG_XSTRINGIFY(__LINE__) }, message)
#define LOG_INFO(message...) Kernel::Log::log(Kernel::Log::Record{ Kernel::Log::INFO, __FILE__, LOG_XSTRINGIFY(__LINE__) }, message)
#define LOG_WARN(message...) Kernel::Log::log(Kernel::Log::Record{ Kernel::Log::WARN, __FILE__, LOG_XSTRINGIFY(__LINE__) }, message)
#define LOG_ERROR(message...) Kernel::Log::log(Kernel::Log::Record{ Kernel::Log::ERROR, __FILE__, LOG_XSTRINGIFY(__LINE__) }, message)

#include <cstdarg>
#include "device/port/serial/SimpleSerialPort.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/async/Spinlock.h"

namespace Kernel {

class Log {

public:

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

    static void setLevel(Level level);

    static void addOutputStream(Util::Io::OutputStream &stream);

    static void removeOutputStream(Util::Io::OutputStream &stream);

    static void disableEarlySerialLogging();

    static void log(const Record &record, const char *message...);

private:

    static void logEarly(const Record &record, const char *message);

    static void logEarlyWithHeap(const Record &record, const char *message...);

    static void writeStringEarly(const char *string);

    static const char* extractFileName(const char *path);

    static const char* getLevelAsString(const Level &level);

    static const char*getColor(const Level &level);

    static Level level;

    static Util::Async::Spinlock lock;
    static Util::HashMap<Util::Io::OutputStream*, Util::Io::PrintStream*> streamMap;
    static Util::ArrayList<Util::String> buffer;

    static Device::SimpleSerialPort *serial;
    static bool serialChecked;
    static bool earlySerialLoggingEnabled;

    static const constexpr char *LEVEL_TRACE = "TRC";
    static const constexpr char *LEVEL_DEBUG = "DBG";
    static const constexpr char *LEVEL_INFO = "INF";
    static const constexpr char *LEVEL_WARN = "WRN";
    static const constexpr char *LEVEL_ERROR = "ERR";
};

}

#endif
