#ifndef __Mutex_include__
#define __Mutex_include__


#include "Lock.h"

class Mutex : public Lock {

public:

    void acquire() override;

    void release() override;

    bool isLocked() override;

private:

    uint32_t lockVar = 0;

    static const uint32_t MUTEX_LOCK = 0x1;

    static const uint32_t MUTEX_UNLOCK = 0x0;

};


#endif
