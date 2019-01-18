#ifndef HHUOS_ACCESSARRAYTHREADPRIORITY_H
#define HHUOS_ACCESSARRAYTHREADPRIORITY_H

#include <lib/util/Array.h>
#include "ThreadPriority.h"

class AccessArrayThreadPriority : public ThreadPriority {

public:

    explicit AccessArrayThreadPriority(uint8_t priorityCount);

    AccessArrayThreadPriority(const AccessArrayThreadPriority &copy) = delete;

    ~AccessArrayThreadPriority() = default;

    uint8_t getNextPriority() override;

    void reset() override;

private:

    Util::Array<uint8_t> accessArray;

    uint8_t currentIndex;

};

#endif
