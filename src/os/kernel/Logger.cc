
#include <lib/libc/printf.h>
#include <kernel/services/StdStreamService.h>
#include "Logger.h"
#include "Kernel.h"

Util::ArrayList<String> Logger::logs;
Logger::LogLevel Logger::currentLevel = LogLevel::DEBUG;

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

    logs.add(tmp);
    if(forcePrint || level >= currentLevel) {
        OutputStream *stream = Kernel::getService<StdStreamService>()->getStdout();
        *stream << tmp << endl;
    }
}

Util::ArrayList<String> &Logger::getLogs() {
    return logs;
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