/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "lib/String.h"
#include "lib/util/ArrayList.h"
#include "lib/time/TimeProvider.h"
#include "devices/ports/Serial.h"
#include "kernel/log/Appender.h"

class Logger {

public:

    Logger(const Logger &other) = delete;

    Logger &operator=(const Logger &other) = delete;

    virtual ~Logger() = default;

    enum LogLevel {
        TRACE = 0x0,
        DEBUG = 0x1,
        INFO  = 0x2,
        WARN  = 0x3,
        ERROR = 0x4
    };

    static Logger& get(const String &name) noexcept;

    void trace(const String &message, ...);

    void debug(const String &message, ...);

    void info(const String &message, ...);

    void warn(const String &message, ...);

    void error(const String &message, ...);

    static void setLevel(LogLevel level);

    static void setLevel(const String &level);

    static void addAppender(Appender *appender);

    static void removeAppender(Appender *appender);

    static void setConsoleLogging(bool enabled);

    static void initialize();

private:

    explicit Logger(const String &name) noexcept;

    const String name;

    static bool logToStdOut;

    static Util::ArrayList<Appender*> appenders;

    static Util::ArrayList<String> buffer;

    static LogLevel currentLevel;

    static TimeProvider *timeProvider;

    static void logMessage(LogLevel level, const String &name, const String &message);

    static const char* getColor(const LogLevel &level);

    static String getLevelAsString(LogLevel level);

    static constexpr const char* LEVEL_TRACE = "trace";
    static constexpr const char* LEVEL_DEBUG = "debug";
    static constexpr const char* LEVEL_INFO = "info";
    static constexpr const char* LEVEL_WARN = "warn";
    static constexpr const char* LEVEL_ERROR = "error";
};


#endif
