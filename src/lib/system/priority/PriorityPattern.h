#ifndef HHUOS_PRIORITYPATTERN_H
#define HHUOS_PRIORITYPATTERN_H

#include <cstdint>

class PriorityPattern {

public:

    explicit PriorityPattern(uint8_t priorityCount);

    PriorityPattern(const PriorityPattern &copy) = delete;

    ~PriorityPattern() = default;

    uint8_t getMaxPriority();

    uint8_t getMinPriority();

    uint8_t getPriorityCount();

    virtual uint8_t getNextPriority() = 0;

    virtual void reset() = 0;

protected:

    uint8_t priorityCount;

};

#endif
