#include <kernel/threads/Scheduler.h>
#include <lib/system/SystemCall.h>
#include "Mutex.h"

void Mutex::acquire() {

    while (!swap(&lockVar, MUTEX_UNLOCK, MUTEX_LOCK)) {

        Cpu::softInterrupt(SystemCall::SCHEDULER_YIELD);
    }
}

void Mutex::release() {

    if (!swap(&lockVar, MUTEX_LOCK, MUTEX_UNLOCK)) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Mutex: Failed to release lock!");
    }
}

bool Mutex::isLocked() {

    return lockVar == 1;
}
