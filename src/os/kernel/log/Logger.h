#ifndef HHUOS_LOGGER_H
#define HHUOS_LOGGER_H

#include <lib/String.h>
#include <lib/util/ArrayList.h>
#include <lib/file/File.h>
#include <devices/Serial.h>
#include <lib/time/TimeProvider.h>
#include "Appender.h"

class Logger {

public:

    enum LogLevel {
        TRACE = 0x0,
        DEBUG = 0x1,
        INFO  = 0x2,
        WARN  = 0x3,
        ERROR = 0x4
    };

    static void trace(const String &message, bool forcePrint = false);

    static void debug(const String &message, bool forcePrint = false);

    static void info(const String &message, bool forcePrint = false);

    static void warn(const String &message, bool forcePrint = false);

    static void error(const String &message, bool forcePrint = false); // Maybe switch to true

    static void setLevel(LogLevel level);

    static void setLevel(const String &level);

    static void addAppender(Appender *appender);

    static void initialize();

private:

    static Util::ArrayList<Appender*> appenders;

    static Util::ArrayList<String> buffer;

    static LogLevel currentLevel;

    static TimeProvider *timeProvider;

    static void logMessage(LogLevel level, const String &message, bool shouldPrint);

    static String getLevelAsString(LogLevel level);

    static constexpr const char* LEVEL_TRACE = "trace";
    static constexpr const char* LEVEL_DEBUG = "debug";
    static constexpr const char* LEVEL_INFO = "info";
    static constexpr const char* LEVEL_WARN = "warn";
    static constexpr const char* LEVEL_ERROR = "error";
};


#endif
