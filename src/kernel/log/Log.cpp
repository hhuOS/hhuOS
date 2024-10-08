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
 
#include "Log.h"

#include <stdint.h>

#include "lib/util/graphic/Ansi.h"
#include "device/port/serial/SerialPort.h"
#include "GatesOfHell.h"
#include "lib/interface.h"
#include "device/port/serial/Serial.h"
#include "device/port/serial/SimpleSerialPort.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/base/Exception.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/time/Timestamp.h"

namespace Kernel {

Log::Level Log::level = Log::INFO;
Util::Async::Spinlock Log::lock{};
Util::HashMap<Util::Io::OutputStream*, Util::Io::PrintStream*> Log::streamMap{};
Util::ArrayList<Util::String> Log::buffer{};
Device::SimpleSerialPort *Log::serial = nullptr;
bool Log::serialChecked = false;
bool Log::earlySerialLoggingEnabled = false;

void Log::setLevel(Level level) {
    Log::level = level;
}

void Log::addOutputStream(Util::Io::OutputStream &stream, bool append) {
    lock.acquire();
    auto *printStream = new Util::Io::PrintStream(stream);

    if (append) {
        for (const auto &message: buffer) {
            *printStream << message << Util::Io::PrintStream::endl;
        }
    }

    streamMap.put(&stream, printStream);
    lock.release();
}

void Log::removeOutputStream(Util::Io::OutputStream &stream) {
    lock.acquire();

    const auto *printStream = streamMap.get(&stream);
    streamMap.remove(&stream);
    delete printStream;

    lock.release();
}

void Log::disableEarlySerialLogging() {
    earlySerialLoggingEnabled = false;
}

void Log::log(const Record &record, const char *message...) {
    if (record.level < level) {
        return;
    }

    va_list args;
    va_start(args, message);

    if (isMemoryManagementInitialized()) {
        logDefault(record, message, args);
    } else if (GatesOfHell::isKernelHeapInitialized()) {
        logEarlyWithHeap(record, message, args);
    } else {
        logEarly(record, message);
    }

    va_end(args);
}

void Log::logDefault(const Log::Record &record, const char *message, va_list args) {
    lock.acquire();

    uint32_t millis = Util::Time::getSystemTime().toMilliseconds();
    uint32_t seconds = millis / 1000;
    uint32_t fraction = millis % 1000;

    const auto logMessage = Util::String::format("%s[%u.%03u]%s[%s]%s[%s:%s] %s%s",
             Util::Graphic::Ansi::FOREGROUND_CYAN, seconds, fraction, getColor(record.level), getLevelAsString(record.level),
             Util::Graphic::Ansi::FOREGROUND_MAGENTA, extractFileName(record.file), record.line,
             Util::Graphic::Ansi::FOREGROUND_DEFAULT, static_cast<const char*>(Util::String::vformat(message, args)));

    buffer.add(logMessage);

    if (earlySerialLoggingEnabled) {
        writeStringEarly(static_cast<const char*>(logMessage));
        writeStringEarly("\n");
    }

    for (auto *stream : streamMap.keys()) {
        auto &printStream = *streamMap.get(stream);
        printStream << logMessage << Util::Io::PrintStream::endl;
    }

    lock.release();
}

void Log::logEarlyWithHeap(const Log::Record &record, const char *message, va_list args) {
    const auto logMessage = Util::String::format("%s[%u.%03u]%s[%s]%s[%s:%s] %s%s\n",
             Util::Graphic::Ansi::FOREGROUND_CYAN, 0, 0, getColor(record.level), getLevelAsString(record.level),
             Util::Graphic::Ansi::FOREGROUND_MAGENTA, extractFileName(record.file), record.line,
             Util::Graphic::Ansi::FOREGROUND_DEFAULT, static_cast<const char*>(Util::String::vformat(message, args)));

    writeStringEarly(static_cast<const char*>(logMessage));
}

void Log::logEarly(const Log::Record &record, const char *message) {
    writeStringEarly(Util::Graphic::Ansi::FOREGROUND_CYAN);
    writeStringEarly("[0.000]");
    writeStringEarly(getColor(record.level));
    writeStringEarly("[");
    writeStringEarly(getLevelAsString(record.level));
    writeStringEarly("]");
    writeStringEarly(Util::Graphic::Ansi::FOREGROUND_MAGENTA);
    writeStringEarly("[");
    writeStringEarly(extractFileName(record.file));
    writeStringEarly(":");
    writeStringEarly(record.line);
    writeStringEarly("] ");

    writeStringEarly(Util::Graphic::Ansi::FOREGROUND_DEFAULT);
    writeStringEarly(message);
    writeStringEarly("\n");
}

void Log::writeStringEarly(const char *string) {
    if (!serialChecked) {
        if (Device::SerialPort::checkPort(Device::Serial::COM1)) {
            static auto serial = Device::SimpleSerialPort(Device::Serial::COM1);
            Kernel::Log::serial = &serial;
        } else if (Device::SerialPort::checkPort(Device::Serial::COM2)) {
            static auto serial = Device::SimpleSerialPort(Device::Serial::COM2);
            Kernel::Log::serial = &serial;
        } else if (Device::SerialPort::checkPort(Device::Serial::COM3)) {
            static auto serial = Device::SimpleSerialPort(Device::Serial::COM3);
            Kernel::Log::serial = &serial;
        } else if (Device::SerialPort::checkPort(Device::Serial::COM4)) {
            static auto serial = Device::SimpleSerialPort(Device::Serial::COM4);
            Kernel::Log::serial = &serial;
        }

        earlySerialLoggingEnabled = (serial != nullptr);
        serialChecked = true;
    }

    if (serial == nullptr) {
        return;
    }

    for (uint32_t i = 0; string[i] != '\0'; i++) {
        serial->write(string[i]);
    }
}

const char *Log::extractFileName(const char *path) {
    const char *filename = path;

    while (*path != '\0') {
        if (*path++ == '/') {
            filename = path;
        }
    }

    return filename;
}

const char* Log::getLevelAsString(const Level &level) {
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

const char* Log::getColor(const Level &level) {
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

void Log::setLevel(const Util::String &level) {
    const auto logLevel = level.toUpperCase();
    if (logLevel == LEVEL_TRACE) {
        setLevel(Level::TRACE);
    } else if (logLevel == LEVEL_DEBUG) {
        setLevel(Level::DEBUG);
    } else if (logLevel == LEVEL_INFO) {
        setLevel(Level::INFO);
    } else if (logLevel == LEVEL_WARN) {
        setLevel(Level::WARN);
    } else if (logLevel == LEVEL_ERROR) {
        setLevel(Level::ERROR);
    } else {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Logger: Invalid log level!");
    }
}

const Device::SimpleSerialPort &Log::getEarlyLogSerialPort() {
    return *serial;
}

Util::String Log::getLog() {
    Util::String log;
    for (const auto &message: buffer) {
        log += message + "\n";
    }

    return log;
}

}