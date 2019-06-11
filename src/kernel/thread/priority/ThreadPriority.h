#ifndef HHUOS_PRIORITY_H
#define HHUOS_PRIORITY_H

#include <cstdint>

namespace Kernel {

class ThreadPriority {

public:

    explicit ThreadPriority(uint8_t priorityCount);

    ThreadPriority(const ThreadPriority &copy) = delete;

    ~ThreadPriority() = default;

    uint8_t getMaxPriority();

    uint8_t getMinPriority();

    uint8_t getPriorityCount();

    virtual uint8_t getNextPriority() = 0;

    virtual void reset() = 0;

protected:

    uint8_t priorityCount;

};

}

#endif
