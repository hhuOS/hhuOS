#ifndef __TimeProvider_include__
#define __TimeProvider_include__

#include <stdint-gcc.h>

class TimeProvider {

public:

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

protected:

    uint32_t ticks = 0;
};

#endif
