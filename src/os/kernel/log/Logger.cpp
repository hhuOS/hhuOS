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

#include <lib/libc/printf.h>
#include <kernel/services/StdStreamService.h>
#include <lib/file/File.h>
#include <devices/timer/Pit.h>
#include <kernel/debug/GdbServer.h>
#include <lib/multiboot/Structure.h>
#include <lib/graphic/Ansi.h>
#include "Logger.h"
#include "SerialAppender.h"

bool Logger::logToStdOut = false;

Logger::LogLevel Logger::currentLevel = LogLevel::DEBUG;

TimeProvider *Logger::timeProvider = Pit::getInstance();

Util::ArrayList<Appender*> Logger::appenders;

Util::ArrayList<String> Logger::buffer;

Logger::Logger(const String &name) : name(name){

}

void Logger::trace(const String &message, ...) {

    va_list args;
    va_start(args, message);

    logMessage(TRACE, name, String::vformat((char *) message, args));

    va_end(args);
}

void Logger::debug(const String &message, ...) {

    va_list args;
    va_start(args, message);

    logMessage(TRACE, name, String::vformat((char *) message, args));

    va_end(args);
}

void Logger::info(const String &message, ...) {

    va_list args;
    va_start(args, message);

    logMessage(TRACE, name, String::vformat((char *) message, args));

    va_end(args);
}

void Logger::warn(const String &message, ...) {

    va_list args;
    va_start(args, message);

    logMessage(TRACE, name, String::vformat((char *) message, args));

    va_end(args);
}

void Logger::error(const String &message, ...) {

    va_list args;
    va_start(args, message);

    logMessage(TRACE, name, String::vformat((char *) message, args));

    va_end(args);
}

void Logger::logMessage(LogLevel level, const String &name, const String &message) {

    if (level < currentLevel) {

        return;
    }

    uint32_t millis = timeProvider->getMillis();

    uint32_t seconds = millis / 1000;

    uint32_t fraction = millis % 1000;

    String tmp = String::format("%s[%d.%03d]%s[%s]%s[%s] %s", Ansi::CYAN, seconds, fraction,
            getColor(level), (char*) getLevelAsString(level), Ansi::RESET, (char*) name, (char*) message);


    buffer.add(tmp);

    if (logToStdOut) {

        *stdout << tmp << endl;
    }

    for (auto appender : appenders) {

        appender->append(tmp);
    }
}

void Logger::setLevel(LogLevel level) {
    currentLevel = level;
}

String Logger::getLevelAsString(LogLevel level) {
    switch (level) {
        case TRACE:
            return "TRACE";
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARN:
            return "WARN";
        case ERROR:
            return "ERROR";
        default:
            return "";
    }
}

void Logger::setLevel(const String &level) {

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
        setLevel(LogLevel::DEBUG);
    }
}

void Logger::addAppender(Appender *appender) {

    for (const auto &message : buffer) {

        appender->append(message);
    }

    appenders.add(appender);
}

void Logger::initialize() {

    if (Multiboot::Structure::getKernelOption("gdb") == "false") {

        Serial *serial = nullptr;

        if(Serial::checkPort(Serial::COM1)) {
            serial = new Serial(Serial::COM1);
        } else if(Serial::checkPort(Serial::COM2)) {
            serial = new Serial(Serial::COM2);
        } else if(Serial::checkPort(Serial::COM3)) {
            serial = new Serial(Serial::COM3);
        } else if(Serial::checkPort(Serial::COM4)) {
            serial = new Serial(Serial::COM4);
        }

        if(serial != nullptr) {
            SerialAppender *serialAppender = new SerialAppender(*serial);

            addAppender(serialAppender);
        }
    }
}

Logger &Logger::get(const String &name) {

    Logger *logger = new Logger(name);

    return *logger;
}

void Logger::removeAppender(Appender *appender) {

    appenders.remove(appender);
}

void Logger::setConsoleLogging(bool enabled) {

    logToStdOut = enabled;
}

const char *Logger::getColor(const Logger::LogLevel &level) {

    switch (level) {
        case TRACE:
            return Ansi::BRIGHT_WHITE;
        case DEBUG:
            return Ansi::BRIGHT_GREEN;
        case INFO:
            return Ansi::BRIGHT_BLUE;
        case WARN:
            return Ansi::BRIGHT_YELLOW;
        case ERROR:
            return Ansi::BRIGHT_RED;
        default:
            return Ansi::BRIGHT_WHITE;
    }
}
