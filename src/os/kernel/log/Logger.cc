
#include <lib/libc/printf.h>
#include <kernel/services/StdStreamService.h>
#include <lib/file/File.h>
#include <devices/Pit.h>
#include <kernel/debug/GdbServer.h>
#include <lib/multiboot/Structure.h>
#include "Logger.h"
#include "SerialAppender.h"

Logger::LogLevel Logger::currentLevel = LogLevel::DEBUG;

TimeProvider *Logger::timeProvider = Pit::getInstance();

Util::ArrayList<Appender*> Logger::appenders;

Util::ArrayList<String> Logger::buffer;

void Logger::trace(const String &message, bool forcePrint) {
    logMessage(TRACE, message, forcePrint);
}

void Logger::debug(const String &message, bool forcePrint) {
    logMessage(DEBUG, message, forcePrint);
}

void Logger::info(const String &message, bool forcePrint) {
    logMessage(INFO, message, forcePrint);
}

void Logger::warn(const String &message, bool forcePrint) {
    logMessage(WARN, message, forcePrint);
}

void Logger::error(const String &message, bool forcePrint) {
    logMessage(ERROR, message, forcePrint);
}

void Logger::logMessage(LogLevel level, const String &message, bool forcePrint) {

    if (level < currentLevel) {

        return;
    }

    uint32_t millis = timeProvider->getMillis();

    uint32_t seconds = millis / 1000;

    uint32_t fraction = millis % 1000;

    String tmp = "[" + String::valueOf(seconds, 10) + "."
                     + String::valueOf(fraction, 10, 4) + "]["
                     + getLevelAsString(level) + "] ";

    tmp += message;

    buffer.add(tmp);

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

        Serial *serial = new Serial(Serial::COM1);

        SerialAppender *serialAppender = new SerialAppender(*serial);

        addAppender(serialAppender);
    }
}
