#include <kernel/Kernel.h>
#include <kernel/services/TimeService.h>
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
        lock.acquire();

        for(uint32_t i = 0; i < interruptHandler.size(); i++) {
            while(interruptHandler.get(i)->hasInterruptData()) {
                interruptHandler.get(i)->parseInterruptData();
            }
        }

        yield();

        lock.release();
    }
}

void InterruptManager::handleDisabledInterrupts() {
    InterruptFrame dummyFrame{};

    for (uint32_t i = 0; i < interruptHandler.size(); i++) {
        interruptHandler.get(i)->trigger(dummyFrame);
    }
}
