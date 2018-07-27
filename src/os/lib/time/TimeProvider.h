#ifndef __TimeProvider_include__
#define __TimeProvider_include__

#include <cstdint>

class TimeProvider {

public:

    struct Time {
        uint32_t seconds = 0;
        uint32_t fraction = 0;

        void addNanos(uint32_t value);

        void addSeconds(uint32_t value);

        bool operator>(const Time &other) const;

        bool operator>=(const Time &other) const;

        bool operator<(const Time &other) const;

        bool operator<=(const Time &other) const;

        uint32_t toNanos();

        uint32_t toMicros();

        uint32_t toMillis();

        uint32_t toSeconds();

        uint32_t toMinutes();

        uint32_t toHours();

        uint32_t toDays();

        uint32_t toYears();
    };

    enum TimeUnit {
        NANO    = 0x01,
        MICRO   = 0x02,
        MILLI   = 0x03,
        SECONDS = 0x04,
        MINUTES = 0x05,
        HOURS   = 0x06,
        DAYS    = 0x07,
        YEARS   = 0x08
    };

    static uint32_t conversionMap[8][8];

    TimeProvider() noexcept = default;

    virtual ~TimeProvider() = default;

    TimeProvider(const TimeProvider &other) = delete;

    TimeProvider &operator=(const TimeProvider &other) = delete;

    virtual uint32_t getNanos() = 0;

    virtual uint32_t getMicros() = 0;

    virtual uint32_t getMillis() = 0;

    virtual uint32_t getSeconds() = 0;

    virtual uint32_t getMinutes() = 0;

    virtual uint32_t getHours() = 0;

    virtual uint32_t getDays() = 0;

    virtual uint32_t getYears() = 0;

    Time getTime() const {

        return time;
    }

    uint32_t getTicks() const {

        return ticks;
    };

    static uint32_t convert(uint32_t value, TimeProvider::TimeUnit from, TimeProvider::TimeUnit to);

protected:

    uint32_t ticks = 0;

    Time time;
};

#endif