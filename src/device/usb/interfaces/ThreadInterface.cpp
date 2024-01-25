#include "ThreadInterface.h"
#include "../../../lib/util/async/Thread.h"
#include "../../../lib/util/time/Timestamp.h"

void mdelay(uint32_t millis){
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(millis));
}

void udelay(uint32_t micro){
    Util::Time::Timestamp t = Util::Time::Timestamp(0,0);
    t.addNanoseconds(micro * 1000);
    Util::Async::Thread::sleep(t);
}

void ndelay(uint32_t nanos){
    Util::Time::Timestamp t = Util::Time::Timestamp(0,0);
    t.addNanoseconds(nanos);
    Util::Async::Thread::sleep(t);
}

void yield_c(){
    Util::Async::Thread::yield();
}