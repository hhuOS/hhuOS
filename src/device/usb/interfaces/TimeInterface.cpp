#include "TimeInterface.h"
#include <cstdint>
#include "../../../kernel/system/System.h"
#include "../../../kernel/service/TimeService.h"
#include "../../../lib/util/time/Timestamp.h"

uint32_t getSystemTimeInMilli(){
    Kernel::TimeService& t = Kernel::System::getService<Kernel::TimeService>();
    return t.getSystemTime().toMilliseconds();
}

uint32_t getSystemTimeInNano(){
    Kernel::TimeService& t = Kernel::System::getService<Kernel::TimeService>();
    return t.getSystemTime().toNanoseconds();
}

uint32_t getSystemTimeInMicro(){
    Kernel::TimeService& t = Kernel::System::getService<Kernel::TimeService>();
    return t.getSystemTime().toMicroseconds();
}

uint32_t addMilis(uint32_t time_in_millis, uint32_t add_time_millis){
    Util::Time::Timestamp c_t = Util::Time::Timestamp::ofMilliseconds(time_in_millis);
    c_t.addNanoseconds(1000 * 1000 * add_time_millis);
    return c_t.toMilliseconds();
}