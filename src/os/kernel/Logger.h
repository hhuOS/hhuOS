#ifndef HHUOS_LOGGER_H
#define HHUOS_LOGGER_H

#include <lib/String.h>
#include <lib/util/ArrayList.h>
#include <lib/file/File.h>
#include <devices/Serial.h>
#include <lib/time/TimeProvider.h>

class Logger {
public:
    enum LogLevel {
        TRACE, DEBUG, INFO, WARN, ERROR
    };

private:
    static File *syslog;
    static Util::ArrayList<String> tmplogs;

    static constexpr const char* LOG_PATH = "/dev/syslog";

    static LogLevel currentLevel;

    static void logMessage(LogLevel level, const String &message, bool shouldPrint);
    static String getLevelAsString(LogLevel level);

    static constexpr const char* LEVEL_TRACE = "trace";
    static constexpr const char* LEVEL_DEBUG = "debug";
    static constexpr const char* LEVEL_INFO = "info";
    static constexpr const char* LEVEL_WARN = "warn";
    static constexpr const char* LEVEL_ERROR = "error";

    static Serial serial;

    static TimeProvider *timeProvider;

public:
    static void trace(const String &message, bool forcePrint = false);

    static void debug(const String &message, bool forcePrint = false);

    static void info(const String &message, bool forcePrint = false);

    static void warn(const String &message, bool forcePrint = false);

    static void error(const String &message, bool forcePrint = false); // Maybe switch to true

    static void setLevel(LogLevel level);

    static void setLevel(const String &level);
};


#endif //HHUOS_LOGGER_H
