
#include <lib/libc/printf.h>
#include <kernel/services/StdStreamService.h>
#include <lib/file/File.h>
#include <devices/Pit.h>
#include "Logger.h"

Logger::LogLevel Logger::currentLevel = LogLevel::DEBUG;
Util::ArrayList<String> Logger::tmplogs;
File* Logger::syslog = nullptr;
Serial Logger::serial(Serial::COM1);
TimeProvider *Logger::timeProvider = Pit::getInstance();

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

    uint32_t millis = timeProvider->getMillis();

    uint32_t seconds = millis / 1000;

    uint32_t fraction = millis % 1000;

    String tmp = "[" + String::valueOf(seconds, 10) + "."
                     + String::valueOf(fraction, 10, 4) + "]["
                     + getLevelAsString(level) + "] ";

    tmp += message;

    serial.sendData((char*) tmp, tmp.length());
    serial.sendChar('\n');

    if(Kernel::isServiceRegistered(FileSystem::SERVICE_NAME)) {
        if (Logger::syslog == nullptr) {
            Logger::syslog = File::open(Logger::LOG_PATH, "a");
            if (Logger::syslog != nullptr) {
                for (const String &log : tmplogs) {
                    *Logger::syslog  << log << endl;
                }
                tmplogs.clear();
            }
        }

        if (Logger::syslog != nullptr) {
            *Logger::syslog << tmp << endl;
        }
    } else {
        tmplogs.add(tmp);
    }

    if(Kernel::isServiceRegistered(StdStreamService::SERVICE_NAME)) {
        if(forcePrint || level >= currentLevel) {
            *stdout << tmp << endl;
        }
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
