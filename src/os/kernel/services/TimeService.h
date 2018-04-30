#ifndef __TimeService_include__
#define __TimeService_include__

#include "kernel/KernelService.h"
#include "devices/Rtc.h"

#include <cstdint>

class TimeService : public KernelService {

public:

    TimeService();

    uint32_t getSystemTime();

    void tick();

    static constexpr char* SERVICE_NAME = "TimeService";

    void msleep(uint32_t ms);

    Rtc* getRTC() {
    	return rtc;
    }

    Rtc::Date getCurrentDate() {
    	return rtc->getCurrentDate();
    }

private:

    uint32_t systemTime = 0;
    Rtc *rtc;

};


#endif
