#ifndef HHUOS_SIMPLETHREADPRIORITY_H
#define HHUOS_SIMPLETHREADPRIORITY_H

#include "ThreadPriority.h"

namespace Kernel {

class SimpleThreadPriority : public ThreadPriority {

public:

    explicit SimpleThreadPriority(uint8_t priorityCount);

    SimpleThreadPriority(const SimpleThreadPriority &copy) = delete;

    ~SimpleThreadPriority() = default;

    uint8_t getNextPriority() override;

    void reset() override;

private:

    uint8_t currentPriority = 0;

};

}

#endif
