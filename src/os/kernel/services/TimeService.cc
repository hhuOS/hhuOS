#include "TimeService.h"

TimeService::TimeService() {
	rtc = new Rtc();
}

uint32_t TimeService::getSystemTime() {
    return systemTime;
}

void TimeService::tick() {
    systemTime++;
}

void TimeService::msleep(uint32_t ms) {
    unsigned long ticks = ms / 10;
    unsigned long st    = systemTime;

    while (systemTime < (st + ticks));
}
