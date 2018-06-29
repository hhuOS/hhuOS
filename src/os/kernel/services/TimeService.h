#ifndef __TimeService_include__
#define __TimeService_include__

#include "kernel/KernelService.h"
#include "devices/Rtc.h"

#include <cstdint>
#include <lib/time/TimeProvider.h>

class TimeService : public KernelService {

public:

    explicit TimeService(TimeProvider *provider);

    uint32_t getSystemTime();

    static constexpr const char* SERVICE_NAME = "TimeService";

    void msleep(uint32_t ms);

    Rtc* getRTC() {
    	return rtc;
    }

    Rtc::Date getCurrentDate() {
    	return rtc->getCurrentDate();
    }

private:

    Rtc *rtc;

    TimeProvider *provider;
};


#endif
