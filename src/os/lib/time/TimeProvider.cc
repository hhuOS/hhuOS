#include "TimeProvider.h"

uint32_t TimeProvider::conversionMap[8][8] = {
        {                1,           1000,     1000000, 1000000000, 1, 1, 1, 1},
        {             1000,              1,        1000,    1000000,    60000000,    3600000000,    1,    1},
        {          1000000,           1000,           1,       1000,       60000,       3600000,       86400000,       1},
        {       1000000000,        1000000,        1000,          1,          60,          3600,         864000,          31536000},
        {      1,       60000000,       60000,         60,           1,            60,           1440,            525600},
        {    1,     3600000000,     3600000,       3600,          60,             1,             24,              8760},
        {   1,    1,    86400000,     864000,        1440,            24,              1,               365},
        {1, 1, 1,   31536000,      525600,          8760,            365,                 1}
};

// Works only on 64-bit systems
//uint32_t TimeProvider::conversionMap[8][8] = {
//        {                1,           1000,     1000000, 1000000000, 60000000000, 3600000000000, 86400000000000, 31536000000000000},
//        {             1000,              1,        1000,    1000000,    60000000,    3600000000,    86400000000,    31536000000000},
//        {          1000000,           1000,           1,       1000,       60000,       3600000,       86400000,       31536000000},
//        {       1000000000,        1000000,        1000,          1,          60,          3600,         864000,          31536000},
//        {      60000000000,       60000000,       60000,         60,           1,            60,           1440,            525600},
//        {    3600000000000,     3600000000,     3600000,       3600,          60,             1,             24,              8760},
//        {   86400000000000,    86400000000,    86400000,     864000,        1440,            24,              1,               365},
//        {31536000000000000, 31536000000000, 31536000000,   31536000,      525600,          8760,            365,                 1}
//};

uint32_t TimeProvider::convert(uint32_t value, TimeProvider::TimeUnit from, TimeProvider::TimeUnit to) {

    if (from == to) {

        return value;
    }

    if (from > to) {

        return conversionMap[from][to] * value;
    }

    return value / conversionMap[from][to];
}

void TimeProvider::Time::addNanos(uint32_t value) {

    fraction += value;

    if (fraction >= 1000000000) {

        fraction -= 1000000000;

        seconds++;
    }
}

void TimeProvider::Time::addSeconds(uint32_t value) {

    this->seconds += seconds;
}

bool TimeProvider::Time::operator>(const TimeProvider::Time &other) const {

    if (seconds > other.seconds) {

        return true;
    }

    return seconds == other.seconds && fraction > other.fraction;
}

bool TimeProvider::Time::operator>=(const TimeProvider::Time &other) const {

    if (seconds > other.seconds) {

        return true;
    }

    return seconds == other.seconds && fraction >= other.fraction;
}

bool TimeProvider::Time::operator<(const TimeProvider::Time &other) const {

    if (seconds < other.seconds) {

        return true;
    }

    return seconds == other.seconds && fraction < other.fraction;
}

bool TimeProvider::Time::operator<=(const TimeProvider::Time &other) const {

    if (seconds < other.seconds) {

        return true;
    }

    return seconds == other.seconds && fraction <= other.fraction;
}

uint32_t TimeProvider::Time::toNanos() {

    return seconds * 1000000000 + fraction;
}

uint32_t TimeProvider::Time::toMicros() {

    return seconds * 1000000 + fraction / 1000;
}

uint32_t TimeProvider::Time::toMillis() {

    return seconds * 1000 + fraction / 1000000;
}

uint32_t TimeProvider::Time::toSeconds() {

    return seconds;
}

uint32_t TimeProvider::Time::toMinutes() {

    return seconds / 60;
}

uint32_t TimeProvider::Time::toHours() {

    return seconds / 3600;
}

uint32_t TimeProvider::Time::toDays() {

    return seconds / 86400;
}

uint32_t TimeProvider::Time::toYears() {

    return seconds / 31536000;
}
