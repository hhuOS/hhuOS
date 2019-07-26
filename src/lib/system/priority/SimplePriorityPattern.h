#ifndef HHUOS_SIMPLEPRIORITYPATTERN_H
#define HHUOS_SIMPLEPRIORITYPATTERN_H

#include "PriorityPattern.h"

class SimplePriorityPattern : public PriorityPattern {

public:

    explicit SimplePriorityPattern(uint8_t priorityCount);

    SimplePriorityPattern(const SimplePriorityPattern &copy) = delete;

    ~SimplePriorityPattern() = default;

    uint8_t getNextPriority() override;

    void reset() override;

private:

    uint8_t currentPriority = 0;

};

#endif
