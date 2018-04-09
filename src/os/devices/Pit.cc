#include <kernel/Kernel.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/interrupts/Pic.h>
#include "kernel/threads/Scheduler.h"
#include "devices/Pit.h"

IOport control(0x43);

IOport data0(0x40);

Pit* Pit::instance = nullptr;

Pit::Pit(uint32_t us) {

    timeService = Kernel::getService<TimeService>();
    graphicsService = Kernel::getService<GraphicsService>();

    setInterval(us);
}

Pit *Pit::getInstance() {

    if(instance == nullptr) {

        instance = new Pit(DEFAULT_INTERVAL);
    }

    return instance;
}

void Pit::setInterval(uint32_t us) {

    uint32_t divisor = (us * 1000) / TIME_BASE;

    control.outb(0x36);

    data0.outb((uint8_t) (divisor & 0xff));

    data0.outb((uint8_t) (divisor >> 8));
}

uint32_t Pit::getInterval() {

    return timerInterval;
}

void Pit::plugin () {

    IntDispatcher::getInstance().assign(32, *this);

    Pic::getInstance()->allow(Pic::Interrupt::PIT);
}

void Pit::trigger () {

    timeService->tick();

    if (Scheduler::getInstance()->isInitialized()) {

        if(schedulerLock.tryLock()) {
            schedulerLock.release();
            Scheduler::getInstance()->yield();
        }
    }
}
