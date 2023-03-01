/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_LOGGER_H
#define HHUOS_LOGGER_H

#include "lib/util/base/String.h"

namespace Util {
namespace Async {
class Spinlock;
}  // namespace Async

template <typename K, typename V> class HashMap;
template <typename T> class ArrayList;

namespace Io {
class OutputStream;
class PrintStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel {

class Logger {

public:

    enum LogLevel {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4
    };

    /**
     * Destructor.
     */
    ~Logger() = default;

    /**
     * Assignment operator.
     */
    Logger &operator=(const Logger &other) = delete;

    static Logger get(const Util::String &name);

    static void setLevel(LogLevel level);

    static void setLevel(Util::String level);

    static void addOutputStream(Util::Io::OutputStream &stream);

    static void removeOutputStream(Util::Io::OutputStream &stream);

    void trace(const Util::String &message, ...);

    void debug(const Util::String &message, ...);

    void info(const Util::String &message, ...);

    void warn(const Util::String &message, ...);

    void error(const Util::String &message, ...);

private:

    explicit Logger(const Util::String &name);

    /**
     * Copy Constructor.
     */
    Logger(const Logger &other) = default;

    static const char* getLevelAsString(const LogLevel &level);

    static const char *getColor(const LogLevel &level);

    static void logMessage(const LogLevel &level, const Util::String &name, const Util::String &message);

    const Util::String name;

    static LogLevel currentLevel;
    static Util::Async::Spinlock lock;
    static Util::HashMap<Util::Io::OutputStream*, Util::Io::PrintStream*> streamMap;
    static Util::ArrayList<Util::String> buffer;

    static const constexpr char *LEVEL_TRACE = "TRC";
    static const constexpr char *LEVEL_DEBUG = "DBG";
    static const constexpr char *LEVEL_INFO = "INF";
    static const constexpr char *LEVEL_WARN = "WRN";
    static const constexpr char *LEVEL_ERROR = "ERR";
};

}

#endif