
#include <lib/libc/printf.h>
#include <kernel/services/StdStreamService.h>
#include <lib/file/File.h>
#include "Logger.h"
#include "Kernel.h"

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
            for (String log : tmplogs) {
                *Logger::syslog  << log << endl;
            }
            tmplogs.clear();
        }

        *Logger::syslog << tmp << endl;
    } else {
        tmplogs.add(tmp);
    }

    if(forcePrint || level >= currentLevel) {
        OutputStream *stream = Kernel::getService<StdStreamService>()->getStdout();
        *stream << tmp << endl;
    }
}

void Logger::setLevel(LogLevel level) {
    currentLevel = level;
}

String Logger::getLevelAsString(LogLevel level) {
    switch (level) {
        case TRACE:
            return String("TRACE");
        case DEBUG:
            return String("DEBUG");
        case INFO:
            return String("INFO");
        case WARN:
            return String("WARN");
        case ERROR:
            return String("ERROR");
        default:
            return String("");
    }
}