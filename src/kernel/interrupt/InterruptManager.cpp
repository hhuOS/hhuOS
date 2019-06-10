#include "kernel/core/Kernel.h"
#include "kernel/service/TimeService.h"
#include "InterruptHandler.h"
#include "InterruptManager.h"

InterruptManager::InterruptManager() : Thread("InterruptManager", 0xff) {

}

InterruptManager &InterruptManager::getInstance() noexcept {
    static InterruptManager instance;

    return instance;
}

void InterruptManager::registerInterruptHandler(InterruptHandler *device) {
    lock.acquire();

    interruptHandler.add(device);

    lock.release();
}

void InterruptManager::deregisterInterruptHandler(InterruptHandler *device) {
    lock.acquire();

    interruptHandler.remove(device);

    lock.release();
}

void InterruptManager::run() {
    while(true) {

        bool doYield;

        do {
            doYield = true;

            lock.acquire();

            for (uint32_t i = 0; i < interruptHandler.size(); i++) {
                if (interruptHandler.get(i)->hasInterruptData()) {
                    doYield = false;

                    interruptHandler.get(i)->parseInterruptData();
                }
            }

            lock.release();
        } while(!doYield);

        yield();
    }
}

void InterruptManager::handleDisabledInterrupts() {
    InterruptFrame dummyFrame{};

    for (uint32_t i = 0; i < interruptHandler.size(); i++) {
        interruptHandler.get(i)->trigger(dummyFrame);
    }
}
