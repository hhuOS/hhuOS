#ifndef HHUOS_LOGGER_H
#define HHUOS_LOGGER_H

#include <lib/String.h>
#include <lib/util/ArrayList.h>
#include <lib/file/File.h>

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

    static void logMessage(LogLevel level, String message, bool shouldPrint);
    static String getLevelAsString(LogLevel level);

public:
    static void trace(String log, bool forcePrint = false);

    static void debug(String log, bool forcePrint = false);

    static void info(String log, bool forcePrint = false);

    static void warn(String log, bool forcePrint = false);

    static void error(String log, bool forcePrint = false); // Maybe switch to true

    static void setLevel(LogLevel level);
};


#endif //HHUOS_LOGGER_H
