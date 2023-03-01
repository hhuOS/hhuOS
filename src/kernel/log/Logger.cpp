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

#include <stdarg.h>
#include <cstdint>

#include "lib/util/graphic/Ansi.h"
#include "kernel/system/System.h"
#include "kernel/service/TimeService.h"
#include "Logger.h"
#include "kernel/log/Logger.h"
#include "lib/util/base/Exception.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/io/stream/OutputStream.h"

namespace Kernel {

Logger::LogLevel Logger::currentLevel = LogLevel::TRACE;
Util::Async::Spinlock Logger::lock;
Util::HashMap<Util::Io::OutputStream*, Util::Io::PrintStream*> Logger::streamMap;
Util::ArrayList<Util::String> Logger::buffer;

Logger::Logger(const Util::String &name) : name(name) {}

Logger Logger::get(const Util::String &name) {
    return Logger(name);
}

void Logger::setLevel(Logger::LogLevel level) {
    lock.acquire();
    currentLevel = level;
    lock.release();
}

void Logger::setLevel(Util::String level) {
    level = level.toUpperCase();
    if (level == LEVEL_TRACE) {
        setLevel(LogLevel::TRACE);
    } else if (level == LEVEL_DEBUG) {
        setLevel(LogLevel::DEBUG);
    } else if (level == LEVEL_INFO) {
        setLevel(LogLevel::INFO);
    } else if (level == LEVEL_WARN) {
        setLevel(LogLevel::WARN);
    } else if (level == LEVEL_ERROR) {
        setLevel(LogLevel::ERROR);
    } else {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Logger: Invalid log level!");
    }
}

void Logger::addOutputStream(Util::Io::OutputStream &stream) {
    lock.acquire();

    auto *printStream = new Util::Io::PrintStream(stream);
    for (const auto &message : buffer) {
        *printStream << message << Util::Io::PrintStream::endl;
    }

    streamMap.put(&stream, printStream);

    lock.release();
}

void Logger::removeOutputStream(Util::Io::OutputStream &stream) {
    lock.acquire();

    const auto *printStream = streamMap.get(&stream);
    streamMap.remove(&stream);
    delete printStream;

    lock.release();
}

void Logger::trace(const Util::String &message, ...) {
    va_list args;
    va_start(args, message);

    logMessage(TRACE, name, Util::String::vformat((char *) message, args));

    va_end(args);
}

void Logger::debug(const Util::String &message, ...) {
    va_list args;
    va_start(args, message);

    logMessage(DEBUG, name, Util::String::vformat((char *) message, args));

    va_end(args);
}

void Logger::info(const Util::String &message, ...) {
    va_list args;
    va_start(args, message);

    logMessage(INFO, name, Util::String::vformat((char *) message, args));

    va_end(args);
}

void Logger::warn(const Util::String &message, ...) {
    va_list args;
    va_start(args, message);

    logMessage(WARN, name, Util::String::vformat((char *) message, args));

    va_end(args);
}

void Logger::error(const Util::String &message, ...) {
    va_list args;
    va_start(args, message);

    logMessage(ERROR, name, Util::String::vformat((char *) message, args));

    va_end(args);
}

void Logger::logMessage(const LogLevel &level, const Util::String &name, const Util::String &message) {
    if (level < currentLevel) {
        return;
    }

    lock.acquire();

    uint32_t millis = System::isServiceRegistered(TimeService::SERVICE_ID) ? Util::Time::getSystemTime().toMilliseconds() : 0;
    uint32_t seconds = millis / 1000;
    uint32_t fraction = millis % 1000;

    const auto logMessage = Util::String::format("%s[%u.%03u]%s[%s]%s[%s] %s",
        Util::Graphic::Ansi::FOREGROUND_CYAN, seconds, fraction,
        getColor(level), getLevelAsString(level),
        Util::Graphic::Ansi::FOREGROUND_DEFAULT, static_cast<const char*>(name), static_cast<const char*>(message));

    buffer.add(logMessage);

    for (auto *stream : streamMap.keys()) {
        auto &printStream= *streamMap.get(stream);
        printStream << logMessage << Util::Io::PrintStream::endl;
    }

    lock.release();
}

const char* Logger::getLevelAsString(const LogLevel &level) {
    switch (level) {
        case TRACE:
            return LEVEL_TRACE;
        case DEBUG:
            return LEVEL_DEBUG;
        case INFO:
            return LEVEL_INFO;
        case WARN:
            return LEVEL_WARN;
        case ERROR:
            return LEVEL_ERROR;
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Logger: Invalid log level!");
    }
}

const char* Logger::getColor(const Logger::LogLevel &level) {
    switch (level) {
        case TRACE:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_WHITE;
        case DEBUG:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_GREEN;
        case INFO:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE;
        case WARN:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_YELLOW;
        case ERROR:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_RED;
        default:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_WHITE;
    }
}

}