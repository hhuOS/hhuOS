
#include <lib/libc/printf.h>
#include <kernel/services/StdStreamService.h>
#include <lib/file/File.h>
#include "Logger.h"

Logger::LogLevel Logger::currentLevel = LogLevel::DEBUG;
Util::ArrayList<String> Logger::tmplogs;
File* Logger::syslog = nullptr;

void Logger::trace(const String log, bool forcePrint) {
    logMessage(TRACE, log, forcePrint);
}

void Logger::debug(const String log, bool forcePrint) {
    logMessage(DEBUG, log, forcePrint);
}

void Logger::info(const String log, bool forcePrint) {
    logMessage(INFO, log, forcePrint);
}

void Logger::warn(const String log, bool forcePrint) {
    logMessage(WARN, log, forcePrint);
}

void Logger::error(const String log, bool forcePrint) {
    logMessage(ERROR, log, forcePrint);
}

void Logger::logMessage(LogLevel level, const String message, bool forcePrint) {
    String tmp = String("[") + getLevelAsString(level) + String("]");
    tmp += message;

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
