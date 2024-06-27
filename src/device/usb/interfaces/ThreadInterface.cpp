#include "ThreadInterface.h"
#include "../../../lib/util/async/Thread.h"
#include "../../../lib/util/time/Timestamp.h"
#include "../controller/UsbRunnable.h"

extern "C"{
#include "../controller/UsbController.h"
}

void mdelay(uint32_t millis){
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(millis));
}

void udelay(uint32_t micro){
    Util::Time::Timestamp t = Util::Time::Timestamp(0,0);
    t.ofNanoseconds(micro * 1000);
    Util::Async::Thread::sleep(t);
}

void ndelay(uint32_t nanos){
    Util::Time::Timestamp t = Util::Time::Timestamp(0,0);
    t.ofNanoseconds(nanos);
    Util::Async::Thread::sleep(t);
}

void yield_c(){
    Util::Async::Thread::yield();
}

void create_thread(char* name, UsbController* controller){
    Util::Async::Thread::createThread(name, new UsbRunnable(controller));
}