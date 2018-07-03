#ifndef HHUOS_LOGGER_H
#define HHUOS_LOGGER_H

#include "lib/String.h"
#include "lib/util/ArrayList.h"
#include "lib/time/TimeProvider.h"
#include "devices/Serial.h"
#include "kernel/log/Appender.h"

class Logger {

public:

    Logger(const Logger &other) = delete;

    Logger &operator=(const Logger &other) = delete;

    virtual ~Logger() = default;

    enum LogLevel {
        TRACE = 0x0,
        DEBUG = 0x1,
        INFO  = 0x2,
        WARN  = 0x3,
        ERROR = 0x4
    };

    static Logger& get(const String &name);

    void trace(const String &message, ...);

    void debug(const String &message, ...);

    void info(const String &message, ...);

    void warn(const String &message, ...);

    void error(const String &message, ...);

    static void setLevel(LogLevel level);

    static void setLevel(const String &level);

    static void addAppender(Appender *appender);

    static void removeAppender(Appender *appender);

    static void initialize();

private:

    explicit Logger(const String &name) noexcept;

    const String name;

    static Util::ArrayList<Appender*> appenders;

    static Util::ArrayList<String> buffer;

    static LogLevel currentLevel;

    static TimeProvider *timeProvider;

    static void logMessage(LogLevel level, const String &name, const String &message);

    static String getLevelAsString(LogLevel level);

    static constexpr const char* LEVEL_TRACE = "trace";
    static constexpr const char* LEVEL_DEBUG = "debug";
    static constexpr const char* LEVEL_INFO = "info";
    static constexpr const char* LEVEL_WARN = "warn";
    static constexpr const char* LEVEL_ERROR = "error";
};


#endif
