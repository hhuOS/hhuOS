#include "MutexInterface.h"
#include "../../../lib/util/async/Spinlock.h"

void new_mutex(Mutex_C* mutex_c){
    mutex_c->acquire_c = &acquire_c;
    mutex_c->try_acquire_c = &try_acquire_c;
    mutex_c->release_c = &release_c;
    mutex_c->isLocked_c = &isLocked_c;

    mutex_c->mutex_pointer = (void*)new Util::Async::Spinlock();
}

void acquire_c(Mutex_C* mutex_c){
    ((Util::Async::Spinlock*)mutex_c->mutex_pointer)->acquire();
}

int try_acquire_c(Mutex_C* mutex_c){
    return (int)((Util::Async::Spinlock*)mutex_c->mutex_pointer)->tryAcquire();
}

void release_c(Mutex_C* mutex_c){
    ((Util::Async::Spinlock*)mutex_c->mutex_pointer)->release();
}

int isLocked_c(Mutex_C* mutex_c){
    return (int)((Util::Async::Spinlock*)mutex_c->mutex_pointer)->isLocked();
}