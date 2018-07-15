#include "TimeService.h"

TimeService::TimeService(TimeProvider *provider) {
	rtc = new Rtc();
    this->provider = provider;
}

uint32_t TimeService::getSystemTime() {
    return provider->getMillis();
}

void TimeService::msleep(uint32_t ms) {
    unsigned long st = getSystemTime();

    while(true) {
        if(getSystemTime() > (st + ms)) {
            break;
        }
    }
}
