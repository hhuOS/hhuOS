#include <kernel/threads/Scheduler.h>
#include "Mutex.h"

void Mutex::acquire() {

    while (!swap(&lockVar, MUTEX_UNLOCK, MUTEX_LOCK)) {

        Scheduler::getInstance()->yield();
    }
}

void Mutex::release() {

    if (!swap(&lockVar, MUTEX_LOCK, MUTEX_UNLOCK)) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }
}

bool Mutex::isLocked() {

    return lockVar == 1;
}
